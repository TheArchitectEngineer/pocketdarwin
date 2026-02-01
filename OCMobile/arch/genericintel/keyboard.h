#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init();
char scancode_to_ascii(uint8_t scancode);

#endif // KEYBOARD_H
