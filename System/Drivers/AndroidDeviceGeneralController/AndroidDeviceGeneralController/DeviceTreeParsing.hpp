//
//  DeviceTreeParsing.hpp
//  AndroidDeviceGeneralController
//
//  Created by Oaky on 06/02/26.
//

#ifndef DeviceTreeParsing_hpp
#define DeviceTreeParsing_hpp

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    uint64_t uart_base;
    uint64_t uart_size;
    int uart_irq;

    uint64_t fb_base;
    uint64_t fb_size;
    int fb_width;
    int fb_height;
    int fb_stride;

    char external_storage_root[128];
    char mobile_environment_root[128];
    int internal_storage_allowed;
} ADGC_BootArgsInfo;

bool ADGC_ParseBootArgs(const char *boot_args, ADGC_BootArgsInfo *out_info);

const char *ADGC_DefaultExternalStorageRoot(void);
const char *ADGC_DefaultMobileEnvironmentRoot(void);

#endif /* DeviceTreeParsing_hpp */
