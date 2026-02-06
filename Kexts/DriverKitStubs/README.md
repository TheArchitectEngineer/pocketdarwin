DriverKit stubs

This folder is a placeholder for DriverKit-based versions of the UART and framebuffer drivers.

Planned:
- UARTDriverDext: DriverKit serial provider wrapping the DTB-mapped UART.
- FramebufferDext: DriverKit display provider using DTB framebuffer nodes.

These are intentionally minimal until the DTB -> IORegistry mapping is fully defined.
