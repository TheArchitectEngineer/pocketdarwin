#include "UARTDriver.hpp"
#include <IOKit/IOLib.h>
#include <IOKit/IORegistryEntry.h>

#define super IOService
OSDefineMetaClassAndStructors(UARTDriver, IOService)

static void log_boot_args(void)
{
    IORegistryEntry *chosen = IORegistryEntry::fromPath("IODeviceTree:/chosen", gIODTPlane);
    if (!chosen) {
        IOLog("PocketDarwin: UARTDriver no /chosen node\\n");
        return;
    }

    OSObject *bootArgs = chosen->getProperty("boot-args");
    if (bootArgs) {
        if (OSString *str = OSDynamicCast(OSString, bootArgs)) {
            IOLog("PocketDarwin: boot-args=%s\\n", str->getCStringNoCopy());
        } else if (OSData *data = OSDynamicCast(OSData, bootArgs)) {
            const char *bytes = (const char *)data->getBytesNoCopy();
            size_t len = data->getLength();
            if (bytes && len > 0) {
                char *tmp = (char *)IOMalloc(len + 1);
                if (tmp) {
                    memcpy(tmp, bytes, len);
                    tmp[len] = '\\0';
                    IOLog("PocketDarwin: boot-args=%s\\n", tmp);
                    IOFree(tmp, len + 1);
                }
            }
        }
    }
    chosen->release();
}

bool UARTDriver::start(IOService *provider)
{
    if (!super::start(provider))
        return false;

    IOLog("PocketDarwin: UARTDriver start (IOKit stub)\\n");
    log_boot_args();

    registerService();
    return true;
}

void UARTDriver::stop(IOService *provider)
{
    IOLog("PocketDarwin: UARTDriver stop\\n");
    super::stop(provider);
}
