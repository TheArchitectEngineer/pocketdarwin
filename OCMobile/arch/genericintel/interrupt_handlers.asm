;; interrupt_handlers.asm

.macro ISR_NOERRCODE isr_num
.global isr\isr_num
isr\isr_num:
    cli
    push byte 0
    push byte \isr_num
    jmp isr_common_stub
.endm

.macro ISR_ERRCODE isr_num
.global isr\isr_num
isr\isr_num:
    cli
    push byte \isr_num
    jmp isr_common_stub
.endm

;; Exceptions with no error code
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8   ;; Double Fault has an error code
ISR_NOERRCODE 9
ISR_ERRCODE   10  ;; Invalid TSS has an error code
ISR_ERRCODE   11  ;; Segment Not Present has an error code
ISR_ERRCODE   12  ;; Stack-Segment Fault has an error code
ISR_ERRCODE   13  ;; General Protection Fault has an error code
ISR_ERRCODE   14  ;; Page Fault has an error code
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

.macro IRQ irq_num, interrupt_num
.global irq\irq_num
irq\irq_num:
    cli
    push byte 0
    push byte \interrupt_num
    jmp irq_common_stub
.endm

;; IRQs. PIC remap sets IRQ0 to INT 32, IRQ1 to INT 33, etc.
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

;; Common stub for ISRs
.extern isr_handler
isr_common_stub:
    pusha                   ;; Push edi, esi, ebp, esp, ebx, edx, ecx, eax
    mov %ds, %ax            ;; Save current data segment
    push %eax               ;; Push ds
    mov $0x10, %ax          ;; Load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    call isr_handler        ;; Call C handler
    pop %eax                ;; Pop ds
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    popa                    ;; Pop edi, esi, ebp, esp, ebx, edx, ecx, eax
    add esp, 8              ;; Clean up interrupt number and error code
    iret                    ;; Return from interrupt

;; Common stub for IRQs
.extern irq_handler
irq_common_stub:
    pusha                   ;; Push edi, esi, ebp, esp, ebx, edx, ecx, eax
    mov %ds, %ax            ;; Save current data segment
    push %eax               ;; Push ds
    mov $0x10, %ax          ;; Load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    call irq_handler        ;; Call C handler
    pop %eax                ;; Pop ds
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    popa                    ;; Pop edi, esi, ebp, esp, ebx, edx, ecx, eax
    add esp, 8              ;; Clean up interrupt number and dummy error code
    iret                    ;; Return from interrupt
