#include "idt.h"
#include <stdint.h>
#include <stddef.h>

// Import inb and outb from kernOC.c
extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t val);

// Define the IDT table
struct idt_entry idt[256];
struct idt_ptr idtp;

// External assembly interrupt handlers
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


// Function to set an IDT entry
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

// Function to remap the PIC
void pic_remap(void) {
    outb(0x20, 0x11); // Start initialization sequence (cascade mode)
    outb(0xA0, 0x11);
    outb(0x21, 0x20); // Master PIC offset 0x20 (32)
    outb(0xA1, 0x28); // Slave PIC offset 0x28 (40)
    outb(0x21, 0x04); // Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 0x02); // Tell Slave PIC its cascade identity (0000 0010)
    outb(0x21, 0x01); // 8086/88 (MCS-80/85) mode
    outb(0xA1, 0x01);
    outb(0x21, 0x0);  // Mask all interrupts on Master PIC
    outb(0xA1, 0x0);  // Mask all interrupts on Slave PIC
}

// Interrupt handler C function
void isr_handler(struct regs *r) {
    // TODO: Implement actual interrupt handling logic
    // For now, just print something to the screen.
    // terminal_writestring("Interrupt!");
}

void irq_handler(struct regs *r) {
    // After an IRQ, we must send an EOI (End of Interrupt) signal to the PICs
    // or they will not send any more interrupts.
    if (r->int_no >= 40) { // If it came from the slave PIC
        outb(0xA0, 0x20); // Send EOI to slave PIC
    }
    outb(0x20, 0x20); // Send EOI to master PIC

    // Call specific IRQ handler if registered
    // For now, only handle keyboard (IRQ1, which is INT 0x21)
    if (r->int_no == 0x21) {
        extern void keyboard_handler(unsigned char scancode);
        unsigned char scancode = inb(0x60);
        keyboard_handler(scancode);
    }
}

// Structure for registers pushed by interrupt handlers
struct regs {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

// Main IDT initialization function
void init_idt() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // Set all IDT gates to a default handler initially (e.g., fault handler)
    // For this example, we'll set up basic ones.

    // Remap the PIC
    pic_remap();

    // ISRs (Exceptions)
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E); // Division By Zero
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E); // Debug
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E); // Non Maskable Interrupt
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E); // Breakpoint
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E); // Overflow
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E); // BOUND Range Exceeded
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E); // Invalid Opcode
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E); // Device Not Available (No Math Coprocessor)
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E); // Double Fault
    idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E); // Coprocessor Segment Overrun
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E); // Invalid TSS
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E); // Segment Not Present
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E); // Stack-Segment Fault
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E); // General Protection Fault
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E); // Page Fault
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E); // (Intel reserved)
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E); // x87 FPU Floating-Point Error
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E); // Alignment Check
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E); // Machine Check
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E); // SIMD Floating-Point Exception
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E); // Virtualization Exception
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E); // Control Protection Exception
    // 22-29 are reserved
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E); // Hypervisor Injection Exception
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E); // VMM Communication Exception

    // IRQs
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);  // IRQ0 (PIT)
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);  // IRQ1 (Keyboard)
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);  // IRQ2 (Cascade)
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);  // IRQ3 (COM2)
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);  // IRQ4 (COM1)
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);  // IRQ5 (LPT2)
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);  // IRQ6 (Floppy)
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);  // IRQ7 (LPT1 / spurious)
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);  // IRQ8 (RTC)
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);  // IRQ9 (Free / ACPI)
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E); // IRQ10 (Free)
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E); // IRQ11 (Free)
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E); // IRQ12 (PS/2 Mouse)
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E); // IRQ13 (FPU / Co-Processor)
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E); // IRQ14 (Primary ATA)
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E); // IRQ15 (Secondary ATA)

    asm volatile("lidt %0" : : "memory"(idtp)); // Load the IDT
    asm volatile("sti"); // Enable interrupts
}
