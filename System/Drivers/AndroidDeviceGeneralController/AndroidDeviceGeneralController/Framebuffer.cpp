//
//  Framebuffer.cpp
//  AndroidDeviceGeneralController
//
//  Created by Oaky on 06/02/26.
//

#include "Framebuffer.hpp"

#include <string.h>

bool ADGC_FramebufferFromBootArgs(const ADGC_BootArgsInfo *boot, ADGC_FramebufferInfo *out_fb)
{
    if (!boot || !out_fb) {
        return false;
    }
    memset(out_fb, 0, sizeof(*out_fb));

    out_fb->base = boot->fb_base;
    out_fb->size = boot->fb_size;
    out_fb->width = boot->fb_width;
    out_fb->height = boot->fb_height;
    out_fb->stride = boot->fb_stride;

    return (out_fb->base != 0 && out_fb->size != 0);
}
