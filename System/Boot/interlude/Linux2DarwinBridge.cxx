#include "Linux2DarwinBridge.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if defined(L2D_USE_LIBFDT)
#include <libfdt.h>
#endif

static void set_error(char **error, const char *message) {
    if (!error || *error) {
        return;
    }
    *error = strdup(message);
}

static char *dup_string(const char *value) {
    if (!value) {
        return NULL;
    }
    size_t len = strlen(value);
    char *out = (char *)malloc(len + 1);
    if (!out) {
        return NULL;
    }
    memcpy(out, value, len);
    out[len] = '\0';
    return out;
}

static void init_result(L2D_BridgeResult *out) {
    if (!out) {
        return;
    }
    memset(out, 0, sizeof(*out));
}

#if defined(L2D_USE_LIBFDT)
static uint64_t read_cells(const fdt32_t *cells, int count) {
    uint64_t value = 0;
    for (int i = 0; i < count; ++i) {
        value = (value << 32) | fdt32_to_cpu(cells[i]);
    }
    return value;
}

static char *get_node_path(const void *fdt, int node) {
    char tmp[256];
    int rc = fdt_get_path(fdt, node, tmp, (int)sizeof(tmp));
    if (rc < 0) {
        return NULL;
    }
    return dup_string(tmp);
}

static void parse_cpus(const void *fdt, L2D_BridgeResult *out) {
    int cpus = fdt_path_offset(fdt, "/cpus");
    if (cpus < 0) {
        return;
    }

    int count = 0;
    int node;
    fdt_for_each_subnode(node, fdt, cpus) {
        const char *dtype = (const char *)fdt_getprop(fdt, node, "device_type", NULL);
        if (dtype && strcmp(dtype, "cpu") == 0) {
            count++;
            continue;
        }
        const char *name = fdt_get_name(fdt, node, NULL);
        if (name && strncmp(name, "cpu@", 4) == 0) {
            count++;
        }
    }
    out->cpu.count = count;
}

static void parse_memory(const void *fdt, L2D_BridgeResult *out) {
    int mem = fdt_path_offset(fdt, "/memory");
    if (mem < 0) {
        return;
    }

    int addr_cells = fdt_address_cells(fdt, 0);
    int size_cells = fdt_size_cells(fdt, 0);
    if (addr_cells <= 0) {
        addr_cells = 1;
    }
    if (size_cells <= 0) {
        size_cells = 1;
    }

    int len = 0;
    const fdt32_t *reg = (const fdt32_t *)fdt_getprop(fdt, mem, "reg", &len);
    if (!reg || len < (addr_cells + size_cells) * (int)sizeof(fdt32_t)) {
        return;
    }

    out->memory.base = read_cells(reg, addr_cells);
    out->memory.size = read_cells(reg + addr_cells, size_cells);
}

static void parse_chosen(const void *fdt, L2D_BridgeResult *out) {
    int chosen = fdt_path_offset(fdt, "/chosen");
    if (chosen < 0) {
        return;
    }

    const char *bootargs = (const char *)fdt_getprop(fdt, chosen, "bootargs", NULL);
    if (bootargs) {
        out->chosen.bootargs = dup_string(bootargs);
    }

    const char *stdout_path = (const char *)fdt_getprop(fdt, chosen, "stdout-path", NULL);
    if (stdout_path) {
        out->chosen.stdout_path = dup_string(stdout_path);
    }
}

static int parse_reg(const void *fdt, int node, L2D_MemoryRange *out_range) {
    int addr_cells = fdt_address_cells(fdt, 0);
    int size_cells = fdt_size_cells(fdt, 0);
    if (addr_cells <= 0) {
        addr_cells = 1;
    }
    if (size_cells <= 0) {
        size_cells = 1;
    }

    int len = 0;
    const fdt32_t *reg = (const fdt32_t *)fdt_getprop(fdt, node, "reg", &len);
    if (!reg || len < (addr_cells + size_cells) * (int)sizeof(fdt32_t)) {
        return -1;
    }
    out_range->base = read_cells(reg, addr_cells);
    out_range->size = read_cells(reg + addr_cells, size_cells);
    return 0;
}

static int parse_irq(const void *fdt, int node) {
    int len = 0;
    const fdt32_t *irq = (const fdt32_t *)fdt_getprop(fdt, node, "interrupts", &len);
    if (!irq || len < (int)sizeof(fdt32_t)) {
        return -1;
    }
    return (int)fdt32_to_cpu(irq[0]);
}

static void fill_uart(const void *fdt, int node, L2D_BridgeResult *out) {
    out->uart.path = get_node_path(fdt, node);
    const char *compat = (const char *)fdt_getprop(fdt, node, "compatible", NULL);
    out->uart.compatible = compat ? dup_string(compat) : NULL;
    parse_reg(fdt, node, &out->uart.reg);
    out->uart.irq = parse_irq(fdt, node);
}

static bool is_uart_compat(const char *compat) {
    if (!compat) {
        return false;
    }
    return strstr(compat, "qcom,msm-uart") ||
           strstr(compat, "qcom,uartdm") ||
           strstr(compat, "serial");
}

static void parse_uart(const void *fdt, L2D_BridgeResult *out) {
    if (out->chosen.stdout_path) {
        int node = fdt_path_offset(fdt, out->chosen.stdout_path);
        if (node >= 0) {
            fill_uart(fdt, node, out);
            return;
        }
    }

    int node = -1;
    int depth = 0;
    while ((node = fdt_next_node(fdt, node, &depth)) >= 0) {
        const char *compat = (const char *)fdt_getprop(fdt, node, "compatible", NULL);
        if (is_uart_compat(compat)) {
            fill_uart(fdt, node, out);
            return;
        }
    }
}

static void fill_framebuffer(const void *fdt, int node, L2D_BridgeResult *out) {
    out->framebuffer.path = get_node_path(fdt, node);
    const char *compat = (const char *)fdt_getprop(fdt, node, "compatible", NULL);
    out->framebuffer.compatible = compat ? dup_string(compat) : NULL;
    parse_reg(fdt, node, &out->framebuffer.reg);

    int len = 0;
    const fdt32_t *width = (const fdt32_t *)fdt_getprop(fdt, node, "width", &len);
    if (width && len >= (int)sizeof(fdt32_t)) {
        out->framebuffer.width = (int)fdt32_to_cpu(width[0]);
    }
    const fdt32_t *height = (const fdt32_t *)fdt_getprop(fdt, node, "height", &len);
    if (height && len >= (int)sizeof(fdt32_t)) {
        out->framebuffer.height = (int)fdt32_to_cpu(height[0]);
    }
    const fdt32_t *stride = (const fdt32_t *)fdt_getprop(fdt, node, "stride", &len);
    if (stride && len >= (int)sizeof(fdt32_t)) {
        out->framebuffer.stride = (int)fdt32_to_cpu(stride[0]);
    }
}

static bool is_framebuffer_compat(const char *compat) {
    if (!compat) {
        return false;
    }
    return strstr(compat, "simple-framebuffer") ||
           strstr(compat, "qcom,mdss-fb") ||
           strstr(compat, "framebuffer");
}

static void parse_framebuffer(const void *fdt, L2D_BridgeResult *out) {
    int node = -1;
    int depth = 0;
    while ((node = fdt_next_node(fdt, node, &depth)) >= 0) {
        const char *compat = (const char *)fdt_getprop(fdt, node, "compatible", NULL);
        if (is_framebuffer_compat(compat)) {
            fill_framebuffer(fdt, node, out);
            return;
        }
    }
}
#endif

int l2d_parse_dtb(const void *dtb, size_t size, L2D_BridgeResult *out, char **error) {
    if (!dtb || size == 0 || !out) {
        set_error(error, "Invalid DTB input");
        return -1;
    }

    init_result(out);

#if defined(L2D_USE_LIBFDT)
    if (fdt_check_header(dtb) != 0) {
        set_error(error, "DTB header check failed");
        return -1;
    }

    parse_cpus(dtb, out);
    parse_memory(dtb, out);
    parse_chosen(dtb, out);
    parse_uart(dtb, out);
    parse_framebuffer(dtb, out);
    return 0;
#else
    (void)dtb;
    (void)size;
    set_error(error, "libfdt not enabled; build with -DL2D_USE_LIBFDT and link libfdt");
    return -1;
#endif
}

void l2d_free_result(L2D_BridgeResult *result) {
    if (!result) {
        return;
    }
    free(result->chosen.bootargs);
    free(result->chosen.stdout_path);
    free(result->uart.path);
    free(result->uart.compatible);
    free(result->framebuffer.path);
    free(result->framebuffer.compatible);
    init_result(result);
}
