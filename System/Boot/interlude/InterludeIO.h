#ifndef POCKETDARWIN_INTERLUDE_IO_H
#define POCKETDARWIN_INTERLUDE_IO_H

#include <stdbool.h>

// Try to attach stdout/stderr to a USB CDC-ACM device (/dev/ttyACM*)
// Returns true if redirected.
bool interlude_attach_usb_serial(const char *preferred_path);

// Initialize framebuffer output (clears to black). Returns true on success.
bool interlude_init_framebuffer(void);

#endif
