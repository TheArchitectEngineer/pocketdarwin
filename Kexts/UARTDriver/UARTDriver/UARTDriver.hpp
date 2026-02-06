#ifndef POCKETDARWIN_UARTDRIVER_HPP
#define POCKETDARWIN_UARTDRIVER_HPP

#include <IOKit/IOService.h>

class UARTDriver : public IOService
{
    OSDeclareDefaultStructors(UARTDriver)

public:
    bool start(IOService *provider) override;
    void stop(IOService *provider) override;
};

#endif
