#include <stdint.h>
#include "interrupts/interrupts.h"

static InterruptDescriptor64 idt[256];

InterruptDescriptor64 interrupts__new_entry(uint16_t segment_selector, void (*handler)(), uint8_t type_attributes, uint8_t ist)
{
    uint64_t handler_pointer = (uint64_t)handler;

    return (InterruptDescriptor64){
        .segment_selector = segment_selector,
        .handler_pointer_low = (uint16_t)handler_pointer,
        .handler_pointer_med = (uint16_t)(handler_pointer >> 16),
        .handler_pointer_high = (uint32_t)(handler_pointer >> 32),
        .type_attributes = (uint8_t)(type_attributes),
        .ist = ist,
        .zero = 0,
    };
}

static inline void lidt(void *base, uint16_t size)
{
    struct
    {
        uint16_t length;
        void *base;
    } __attribute__((packed)) IDTR = {size, base};

    asm("lidt %0" : : "m"(IDTR)); // let the compiler choose an addressing mode
}

struct interrupt_frame;

__attribute__((interrupt)) void handle_breakpoint(struct interrupt_frame *frame)
{
    printf("Breakpoint\n");
}

__attribute__((interrupt)) void handle_div_zero(struct interrupt_frame *frame)
{
    panic("EXCEPTION_DIV_ZERO");
}

__attribute__((interrupt)) void handle_page_fault(struct interrupt_frame *frame, uint64_t error_code)
{
    panic("EXCEPTION_PAGE_FAULT");
}
__attribute__((interrupt)) void
handle_double_fault(struct interrupt_frame *frame, uint64_t error_code)
{
    panic("EXCEPTION_DOUBLE_FAULT");
}

void init_interrupts()
{
    uint16_t code_segment;

    asm("mov %%cs, %0" : "=r"(code_segment));

    uint16_t segment_selector = code_segment | 0x000; // GDT, privilege level 0

    idt[0] = interrupts__new_entry(segment_selector, handle_div_zero, TRAP_GATE, 0);
    idt[3] = interrupts__new_entry(segment_selector, handle_breakpoint, TRAP_GATE, 0);
    idt[8] = interrupts__new_entry(segment_selector, handle_double_fault, TRAP_GATE, 0);
    idt[14] = interrupts__new_entry(segment_selector, handle_page_fault, TRAP_GATE, 0);

    lidt(idt, 255);
}
