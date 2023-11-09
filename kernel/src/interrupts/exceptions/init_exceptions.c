#include "interrupts/idt.h"
#include <stdint.h>
#include "debug/include.h"

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

__attribute__((interrupt)) void
handle_general_protection_fault(struct interrupt_frame *frame, uint64_t error_code)
{
    panic("EXCEPTION_GENERAL_PROTECTION_FAULT");
}

void init_exceptions(InterruptDescriptor64 idt[256], uint16_t segment_selector)
{
    idt[0] = interrupts__new_entry(segment_selector, handle_div_zero, TRAP_GATE, 0);
    idt[3] = interrupts__new_entry(segment_selector, handle_breakpoint, TRAP_GATE, 0);
    idt[8] = interrupts__new_entry(segment_selector, handle_double_fault, TRAP_GATE, 0);
    idt[13] = interrupts__new_entry(segment_selector, handle_general_protection_fault, TRAP_GATE, 0);
    idt[14] = interrupts__new_entry(segment_selector, handle_page_fault, TRAP_GATE, 0);
}