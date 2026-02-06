#ifndef POCKETDARWIN_LINUX2DARWINBRIDGE_H
#define POCKETDARWIN_LINUX2DARWINBRIDGE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t base;
    uint64_t size;
} L2D_MemoryRange;

typedef struct {
    int count;
} L2D_CPUInfo;

typedef struct {
    char *bootargs;
    char *stdout_path;
} L2D_Chosen;

typedef struct {
    char *path;
    char *compatible;
    L2D_MemoryRange reg;
    int irq;
} L2D_UARTInfo;

typedef struct {
    char *path;
    char *compatible;
    L2D_MemoryRange reg;
    int width;
    int height;
    int stride;
} L2D_FramebufferInfo;

typedef struct {
    L2D_CPUInfo cpu;
    L2D_MemoryRange memory;
    L2D_Chosen chosen;
    L2D_UARTInfo uart;
    L2D_FramebufferInfo framebuffer;
} L2D_BridgeResult;

int l2d_parse_dtb(const void *dtb, size_t size, L2D_BridgeResult *out, char **error);
void l2d_free_result(L2D_BridgeResult *result);

#ifdef __cplusplus
}
#endif

#endif
