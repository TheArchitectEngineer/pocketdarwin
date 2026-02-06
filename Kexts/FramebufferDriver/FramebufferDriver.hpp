#ifndef POCKETDARWIN_FRAMEBUFFERDRIVER_HPP
#define POCKETDARWIN_FRAMEBUFFERDRIVER_HPP

#include <IOKit/IOService.h>

class FramebufferDriver : public IOService
{
    OSDeclareDefaultStructors(FramebufferDriver)

public:
    bool start(IOService *provider) override;
    void stop(IOService *provider) override;
};

#endif
