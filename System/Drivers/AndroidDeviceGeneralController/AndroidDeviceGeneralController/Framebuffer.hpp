//
//  Framebuffer.hpp
//  AndroidDeviceGeneralController
//
//  Created by Oaky on 06/02/26.
//

#ifndef Framebuffer_hpp
#define Framebuffer_hpp

#include <stdint.h>
#include <stdbool.h>

#include "DeviceTreeParsing.hpp"

typedef struct {
    uint64_t base;
    uint64_t size;
    int width;
    int height;
    int stride;
} ADGC_FramebufferInfo;

bool ADGC_FramebufferFromBootArgs(const ADGC_BootArgsInfo *boot, ADGC_FramebufferInfo *out_fb);

#endif /* Framebuffer_hpp */
