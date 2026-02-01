#include "keyboard.h"
#include "../../../../kernel/terminal/terminal.h"
#include <stdint.h>

// Import inb and outb from kernOC.c
extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t val);

// Scancode to ASCII mapping (simplified for common keys)
static unsigned char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, /* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', /* Left Shift */
    'm', ',', '.', '/', 0, 0, 0, ' ', 0, 0, /* Right Shift, Alt, Caps Lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

char scancode_to_ascii(uint8_t scancode) {
    if (scancode < 128) {
        return kbd_us[scancode];
    }
    return 0;
}

void keyboard_init() {
    // Enable keyboard interrupts (unmask IRQ1 on PIC)
    uint8_t master_mask = inb(0x21);
    outb(0x21, master_mask & ~0x02); // Clear the second bit (IRQ1)

    // Send command to keyboard controller to enable keyboard
    while ((inb(0x64) & 0x02) != 0); // Wait for input buffer to be empty
    outb(0x64, 0xAE); // Enable keyboard

    while ((inb(0x64) & 0x02) != 0); // Wait for input buffer to be empty
    outb(0x64, 0x20); // Get current command byte
    while ((inb(0x64) & 0x01) == 0); // Wait for output buffer to be full
    uint8_t status = inb(0x60);
    status = (status | 0x01) & ~0x10; // Enable interrupts, disable translation
    while ((inb(0x64) & 0x02) != 0); // Wait for input buffer to be empty
    outb(0x64, 0x60); // Set command byte
    while ((inb(0x64) & 0x02) != 0); // Wait for input buffer to be empty
    outb(0x60, status);
}
