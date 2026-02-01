#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This kernel requires a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) 
{
	if (c == '\b') { // Handle backspace character
		if (terminal_column > 0) {
			terminal_column--;
			terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
		} else if (terminal_row > 0) { // If at the start of a line, move to the end of the previous line
			terminal_row--;
			terminal_column = VGA_WIDTH - 1;
			terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
		}
		return;
	}

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void outb(uint16_t port, uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

// Corrected typo in enable_block_cursor
void enable_block_cursor() {
	outb(0x3D4, 0x0A); // Cursor start register
	outb(0x3D5, (inb(0x3D5) & 0xC0) | 0x00); // Set cursor start to line 0 (first half of cursor)
	outb(0x3D4, 0x0B); // Cursor end register
	outb(0x3D5, (inb(0x3D5) & 0xE0) | 0x0F); // Set cursor end to line 15 (second half of cursor)
}

// Direct VGA write, for simplicity in prompt printing
void putc(char c) {
	volatile char *vga = (volatile char*)0xB8000;
	static int pos = 0;
	vga[pos++] = c;
	vga[pos++] = 0x07;
}

void puts(const char *s) {
	while (*s) putc(*s++);
}

char input[128];
int input_len = 0; // Use input_len to track current input length. Set to -1 to signal line ready.

// External declaration for scancode_to_ascii from keyboard.c
extern char scancode_to_ascii(uint8_t scancode);

// Modified keyboard_handler to correctly handle input, backspace, newline, and signal readiness.
void keyboard_handler(unsigned char scancode) {
    // Handle Backspace (scancode 0x0E)
    if (scancode == 0x0E) {
        if (input_len > 0) {
            input_len--;
            // Move cursor back, overwrite with space, move cursor back again
            terminal_putchar('\b'); // Move cursor back
            terminal_putchar(' ');  // Overwrite with space
            terminal_putchar('\b'); // Move cursor back again
        }
        return; // Processed backspace, exit handler
    }

    // Handle Enter (scancode 0x1C)
    if (scancode == 0x1C) {
        input[input_len] = '\0'; // Null-terminate the string
        terminal_putchar('\n');  // Print newline to console
        input_len = -1;          // Signal that a line is ready to be processed
        return; // Processed enter, exit handler
    }

    // Handle printable characters
    char c = scancode_to_ascii(scancode);
    if (c != 0) { // Only process if scancode_to_ascii returned a character
        if (input_len < sizeof(input) - 1) { // Check for buffer overflow
            input[input_len++] = c;
            terminal_putchar(c); // Print character to console using terminal interface
        }
    }
}

// Redefined read_line to be a blocking read that waits for a newline and processes input.
void read_line() {
    // The keyboard handler sets input_len to -1 when a newline is pressed and processed.
    // We loop here until input_len becomes -1.
    while (input_len != -1) {
        // This loop will effectively yield control until an interrupt (keyboard) changes input_len.
        // In a real OS, this would be an idle loop or yield. For a simple kernel, an empty loop is sufficient.
        asm volatile("hlt"); // Halt the CPU until the next interrupt
    }

    // Input is ready (input_len is -1)
    // Call boot_from_path to process the command in the 'input' buffer
    // This function will be uncommented and fully implemented later.
    // boot_from_path(input);

    // After processing, reset for the next input
    input_len = 0;
    // Clear the input buffer (optional, but good practice)
    for (size_t i = 0; i < sizeof(input); ++i) {
        input[i] = 0;
    }
}

void boot_from_path() {}

void bootld() {
	const int kernelver = 0x0000001;
	terminal_writestring("OCM littlekernel v");
	// Hardcoding version string as printing integers requires more standard library functions
	terminal_writestring("1.0.0\n"); // Assuming version 1.0.0

	while (1) {
		terminal_writestring("boot: "); // Use terminal interface for prompt
		read_line(); // Wait for user input
		// After read_line returns, 'input' buffer contains the command.
		boot_from_path(); // Process the command (input buffer is global and used by boot_from_path)
	}
}

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	enable_block_cursor();

	terminal_writestring("[WARNING] OpenCore Mobile was builded with Intel CPUs as the target, some features may not work, and OCM will direcly jump to the darwin bootloader.\n");
	terminal_writestring("Darwin/Pocket is now loading....\n");

	/* Initialize Interrupt Descriptor Table and Programmable Interrupt Controller */
	extern void init_idt();
	init_idt();

	/* Initialize keyboard driver */
	extern void keyboard_init();
	keyboard_init();

	bootld();
}
