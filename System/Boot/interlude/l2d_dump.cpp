#include "Linux2DarwinBridge.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static void usage(const char *prog) {
    std::fprintf(stderr, "Usage: %s <path-to.dtb>\n", prog);
}

static void print_range(const char *label, const L2D_MemoryRange &range) {
    std::printf("%s: base=0x%llx size=0x%llx\n",
                label,
                static_cast<unsigned long long>(range.base),
                static_cast<unsigned long long>(range.size));
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    const char *path = argv[1];
    FILE *f = std::fopen(path, "rb");
    if (!f) {
        std::perror("fopen");
        return 1;
    }

    if (std::fseek(f, 0, SEEK_END) != 0) {
        std::perror("fseek");
        std::fclose(f);
        return 1;
    }
    long len = std::ftell(f);
    if (len <= 0) {
        std::fprintf(stderr, "Invalid DTB size.\n");
        std::fclose(f);
        return 1;
    }
    std::rewind(f);

    void *buf = std::malloc(static_cast<size_t>(len));
    if (!buf) {
        std::fprintf(stderr, "Out of memory.\n");
        std::fclose(f);
        return 1;
    }

    size_t read_len = std::fread(buf, 1, static_cast<size_t>(len), f);
    std::fclose(f);
    if (read_len != static_cast<size_t>(len)) {
        std::fprintf(stderr, "Failed to read DTB.\n");
        std::free(buf);
        return 1;
    }

    L2D_BridgeResult result;
    char *error = nullptr;
    int rc = l2d_parse_dtb(buf, read_len, &result, &error);
    std::free(buf);

    if (rc != 0) {
        std::fprintf(stderr, "Parse failed: %s\n", error ? error : "unknown error");
        std::free(error);
        return 1;
    }

    std::printf("CPU count: %d\n", result.cpu.count);
    print_range("Memory", result.memory);
    if (result.chosen.bootargs) {
        std::printf("Bootargs: %s\n", result.chosen.bootargs);
    }
    if (result.chosen.stdout_path) {
        std::printf("Stdout-path: %s\n", result.chosen.stdout_path);
    }

    if (result.uart.path || result.uart.compatible) {
        std::printf("UART path: %s\n", result.uart.path ? result.uart.path : "(none)");
        std::printf("UART compatible: %s\n", result.uart.compatible ? result.uart.compatible : "(none)");
        print_range("UART reg", result.uart.reg);
        std::printf("UART irq: %d\n", result.uart.irq);
    } else {
        std::printf("UART: not found\n");
    }

    if (result.framebuffer.path || result.framebuffer.compatible) {
        std::printf("Framebuffer path: %s\n", result.framebuffer.path ? result.framebuffer.path : "(none)");
        std::printf("Framebuffer compatible: %s\n", result.framebuffer.compatible ? result.framebuffer.compatible : "(none)");
        print_range("Framebuffer reg", result.framebuffer.reg);
        std::printf("Framebuffer width: %d\n", result.framebuffer.width);
        std::printf("Framebuffer height: %d\n", result.framebuffer.height);
        std::printf("Framebuffer stride: %d\n", result.framebuffer.stride);
    } else {
        std::printf("Framebuffer: not found\n");
    }

    l2d_free_result(&result);
    std::free(error);
    return 0;
}
