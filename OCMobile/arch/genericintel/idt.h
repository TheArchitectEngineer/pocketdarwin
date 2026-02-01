#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// Structure for an IDT entry
struct idt_entry {
    uint16_t base_low;    // The lower 16 bits of the address to jump to
    uint16_t sel;         // Kernel segment selector
    uint8_t always0;      // This must always be 0
    uint8_t flags;        // Flags (type, DPL, present)
    uint16_t base_high;   // The upper 16 bits of the address to jump to
} __attribute__((packed));

// Structure for the IDT pointer
struct idt_ptr {
    uint16_t limit;       // Size of the IDT - 1
    uint32_t base;        // Address of the first IDT entry
} __attribute__((packed));

// Function to initialize the IDT
void init_idt();

// Function to set an IDT entry
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif // IDT_H
