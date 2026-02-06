#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

#include "InterludeIO.h"
#include "Linux2DarwinBridge.h"
#include "JSONParser.h"

static const char *default_config_path = "/sdcard/OCMobileConfig.json";
static const char *default_usb_serial = "/dev/ttyACM0";

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [--kernel <path>] [--config <path>] [--initrd <path>] [--cmdline <string>] [--dtb <path>] [--exec] [--dry-run]\n"
        "\n"
        "Prepares a Linux system and loads a Darwin kernel via kexec.\n"
        "\n"
        "Options:\n"
        "  --kernel <path>   Path to Darwin kernel image (optional if --config is used)\n"
        "  --config <path>   Path to OCMobileConfig.json (default: /sdcard/OCMobileConfig.json)\n"
        "  --initrd <path>   Optional initrd/ramdisk image\n"
        "  --cmdline <str>   Kernel command line string\n"
        "  --dtb <path>      Optional Linux DTB for UART/FB handoff\n"
        "  --exec            Execute kexec immediately after loading\n"
        "  --dry-run         Print actions without executing kexec\n"
        "  -h, --help        Show this help\n",
        prog);
}

static bool file_exists(const char *path) {
    struct stat st;
    return path && stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int write_sysctl(const char *path, const char *value) {
    FILE *f = fopen(path, "w");
    if (!f) {
        return -1;
    }
    if (fprintf(f, "%s", value) < 0) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

static char *load_kernel_path_from_config(const char *config_path, char **error) {
    JSONValue *root = json_parse_file(config_path, error);
    if (!root) {
        return NULL;
    }
    char *path = oc_mobile_get_kernel_path(root);
    json_free(root);
    return path;
}

static void *read_file_blob(const char *path, size_t *out_size) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }
    long len = ftell(f);
    if (len <= 0) {
        fclose(f);
        return NULL;
    }
    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    void *buf = malloc((size_t)len);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    size_t read_len = fread(buf, 1, (size_t)len, f);
    fclose(f);
    if (read_len != (size_t)len) {
        free(buf);
        return NULL;
    }
    if (out_size) {
        *out_size = (size_t)len;
    }
    return buf;
}

static char *append_bootargs(const char *base, const char *extra) {
    if (!extra || extra[0] == '\0') {
        return base ? strdup(base) : NULL;
    }
    if (!base || base[0] == '\0') {
        return strdup(extra);
    }
    size_t base_len = strlen(base);
    size_t extra_len = strlen(extra);
    char *out = (char *)malloc(base_len + 1 + extra_len + 1);
    if (!out) {
        return NULL;
    }
    memcpy(out, base, base_len);
    out[base_len] = ' ';
    memcpy(out + base_len + 1, extra, extra_len);
    out[base_len + 1 + extra_len] = '\0';
    return out;
}

static char *build_pd_bootargs(const L2D_BridgeResult *res) {
    char buf[512];
    int offset = 0;
    offset += snprintf(buf + offset, sizeof(buf) - offset,
                       "pd.uart.base=0x%llx pd.uart.size=0x%llx pd.uart.irq=%d",
                       (unsigned long long)res->uart.reg.base,
                       (unsigned long long)res->uart.reg.size,
                       res->uart.irq);
    if (offset < 0 || (size_t)offset >= sizeof(buf)) {
        return NULL;
    }
    offset += snprintf(buf + offset, sizeof(buf) - offset,
                       " pd.fb.base=0x%llx pd.fb.size=0x%llx pd.fb.w=%d pd.fb.h=%d pd.fb.stride=%d",
                       (unsigned long long)res->framebuffer.reg.base,
                       (unsigned long long)res->framebuffer.reg.size,
                       res->framebuffer.width,
                       res->framebuffer.height,
                       res->framebuffer.stride);
    if (offset < 0 || (size_t)offset >= sizeof(buf)) {
        return NULL;
    }
    return strdup(buf);
}

static char *build_pd_storage_bootargs(const char *sd_root, const char *mobile_root, int allow_internal) {
    char buf[512];
    int offset = 0;
    if (sd_root && sd_root[0] != '\0') {
        offset += snprintf(buf + offset, sizeof(buf) - offset, " pd.sdroot=%s", sd_root);
    }
    if (offset < 0 || (size_t)offset >= sizeof(buf)) {
        return NULL;
    }
    if (mobile_root && mobile_root[0] != '\0') {
        offset += snprintf(buf + offset, sizeof(buf) - offset, " pd.mobileenv=%s", mobile_root);
    }
    if (offset < 0 || (size_t)offset >= sizeof(buf)) {
        return NULL;
    }
    if (allow_internal >= 0) {
        offset += snprintf(buf + offset, sizeof(buf) - offset, " pd.allow_internal=%d", allow_internal ? 1 : 0);
    }
    if (offset < 0 || (size_t)offset >= sizeof(buf)) {
        return NULL;
    }
    if (offset == 0) {
        return NULL;
    }
    return strdup(buf);
}

static bool is_linux(void) {
    struct utsname uts;
    if (uname(&uts) != 0) {
        return false;
    }
    return strcmp(uts.sysname, "Linux") == 0;
}

static const char *find_kexec(void) {
    const char *candidates[] = {
        "/sbin/kexec",
        "/usr/sbin/kexec",
        "/bin/kexec",
        "/usr/bin/kexec",
        NULL
    };
    for (int i = 0; candidates[i]; ++i) {
        if (access(candidates[i], X_OK) == 0) {
            return candidates[i];
        }
    }
    return "kexec";
}

int main(int argc, char **argv) {
    const char *kernel = NULL;
    const char *config_path = default_config_path;
    const char *initrd = NULL;
    const char *cmdline = NULL;
    bool do_exec = false;
    bool dry_run = false;
    char *kernel_from_config = NULL;
    char *json_error = NULL;
    char *sd_root = NULL;
    char *mobile_root = NULL;
    int allow_internal = -1;
    bool usb_serial_ok = false;
    bool fb_ok = false;
    const char *dtb_path = NULL;

    if (argc == 1) {
        usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--kernel") == 0 && i + 1 < argc) {
            kernel = argv[++i];
        } else if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
            config_path = argv[++i];
        } else if (strcmp(argv[i], "--initrd") == 0 && i + 1 < argc) {
            initrd = argv[++i];
        } else if (strcmp(argv[i], "--cmdline") == 0 && i + 1 < argc) {
            cmdline = argv[++i];
        } else if (strcmp(argv[i], "--dtb") == 0 && i + 1 < argc) {
            dtb_path = argv[++i];
        } else if (strcmp(argv[i], "--exec") == 0) {
            do_exec = true;
        } else if (strcmp(argv[i], "--dry-run") == 0) {
            dry_run = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (!kernel) {
        kernel_from_config = load_kernel_path_from_config(config_path, &json_error);
        if (!kernel_from_config) {
            fprintf(stderr, "No --kernel provided and failed to read %s", config_path);
            if (json_error) {
                fprintf(stderr, ": %s", json_error);
            }
            fprintf(stderr, ".\n");
            free(json_error);
            return 1;
        }
        kernel = kernel_from_config;
    }

    // Optional storage overrides from OCMobileConfig.json
    JSONValue *config_root = json_parse_file(config_path, NULL);
    if (config_root) {
        sd_root = oc_mobile_get_external_storage_root(config_root);
        mobile_root = oc_mobile_get_mobile_environment_root(config_root);
        allow_internal = oc_mobile_get_internal_storage_allowed(config_root, -1);
        json_free(config_root);
    }

    if (!is_linux()) {
        fprintf(stderr, "This program must run on Linux.\n");
        return 1;
    }

    if (geteuid() != 0) {
        fprintf(stderr, "Please run as root.\n");
        return 1;
    }

    usb_serial_ok = interlude_attach_usb_serial(default_usb_serial);
    if (!usb_serial_ok) {
        fprintf(stderr, "[interlude] USB serial not available, continuing without it.\n");
    }

    fb_ok = interlude_init_framebuffer();
    if (!fb_ok) {
        fprintf(stderr, "[interlude] Framebuffer not available, continuing without it.\n");
    }

    if (!file_exists(kernel)) {
        fprintf(stderr, "Kernel image not found: %s\n", kernel);
        free(kernel_from_config);
        free(json_error);
        return 1;
    }

    if (initrd && !file_exists(initrd)) {
        fprintf(stderr, "Initrd image not found: %s\n", initrd);
        free(kernel_from_config);
        free(json_error);
        return 1;
    }

    const char *kexec_disable = "/proc/sys/kernel/kexec_load_disabled";
    if (access(kexec_disable, F_OK) == 0) {
        if (!dry_run && write_sysctl(kexec_disable, "0") != 0) {
            fprintf(stderr, "Failed to enable kexec load (%s): %s\n", kexec_disable, strerror(errno));
            free(kernel_from_config);
            free(json_error);
            return 1;
        }
    }

    const char *kexec_path = find_kexec();
    if (strcmp(kexec_path, "kexec") == 0) {
        fprintf(stderr, "Warning: kexec not found in common locations; relying on PATH.\n");
    }

    printf("Preparing to load Darwin kernel via kexec...\n");
    printf("  kernel: %s\n", kernel);
    if (initrd) {
        printf("  initrd: %s\n", initrd);
    }
    if (cmdline) {
        printf("  cmdline: %s\n", cmdline);
    }
    if (dtb_path) {
        printf("  dtb: %s\n", dtb_path);
    }
    printf("  exec after load: %s\n", do_exec ? "yes" : "no");

    if (dry_run) {
        printf("Dry run: not invoking kexec.\n");
        free(kernel_from_config);
        free(json_error);
        return 0;
    }

    char *bootargs = cmdline ? strdup(cmdline) : NULL;
    if (dtb_path) {
#if defined(L2D_USE_LIBFDT)
        size_t dtb_size = 0;
        void *dtb = read_file_blob(dtb_path, &dtb_size);
        if (!dtb) {
            fprintf(stderr, "Failed to read DTB: %s\n", dtb_path);
        } else {
            L2D_BridgeResult bridge;
            char *dtb_error = NULL;
            if (l2d_parse_dtb(dtb, dtb_size, &bridge, &dtb_error) == 0) {
                char *extra = build_pd_bootargs(&bridge);
                char *tmp = append_bootargs(bootargs, extra);
                free(bootargs);
                bootargs = tmp;
                free(extra);
                l2d_free_result(&bridge);
            } else {
                fprintf(stderr, "DTB parse failed: %s\n", dtb_error ? dtb_error : "unknown error");
                free(dtb_error);
            }
            free(dtb);
        }
#else
        fprintf(stderr, "DTB support not enabled. Rebuild with L2D_WITH_LIBFDT=1.\n");
#endif
    }

    char *storage_args = build_pd_storage_bootargs(sd_root, mobile_root, allow_internal);
    if (storage_args) {
        char *tmp = append_bootargs(bootargs, storage_args);
        free(bootargs);
        bootargs = tmp;
        free(storage_args);
    }

    const char *effective_cmdline = cmdline;
    if (bootargs) {
        effective_cmdline = bootargs;
    }

    // Build kexec arguments
    char *args[10];
    int idx = 0;
    args[idx++] = (char *)kexec_path;
    args[idx++] = (char *)"-l";
    args[idx++] = (char *)kernel;
    if (initrd) {
        args[idx++] = (char *)"--initrd";
        args[idx++] = (char *)initrd;
    }
    if (effective_cmdline) {
        args[idx++] = (char *)"--command-line";
        args[idx++] = (char *)effective_cmdline;
    }
    args[idx] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed: %s\n", strerror(errno));
        free(kernel_from_config);
        free(json_error);
        free(bootargs);
        free(sd_root);
        free(mobile_root);
        return 1;
    }
    if (pid == 0) {
        execvp(kexec_path, args);
        fprintf(stderr, "Failed to exec kexec: %s\n", strerror(errno));
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        fprintf(stderr, "waitpid failed: %s\n", strerror(errno));
        free(kernel_from_config);
        free(json_error);
        free(bootargs);
        free(sd_root);
        free(mobile_root);
        return 1;
    }
    if (status != 0) {
        fprintf(stderr, "kexec load failed (status %d).\n", status);
        free(kernel_from_config);
        free(json_error);
        free(bootargs);
        free(sd_root);
        free(mobile_root);
        return 1;
    }

    if (do_exec) {
        printf("Executing kexec...\n");
        char *exec_args[] = { (char *)kexec_path, (char *)"-e", NULL };
        execvp(kexec_path, exec_args);
        fprintf(stderr, "Failed to exec kexec -e: %s\n", strerror(errno));
        free(kernel_from_config);
        free(json_error);
        free(bootargs);
        free(sd_root);
        free(mobile_root);
        return 1;
    }

    free(kernel_from_config);
    free(json_error);
    free(bootargs);
    free(sd_root);
    free(mobile_root);
    printf("kexec load complete.\n");
    return 0;
}
