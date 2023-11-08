#include <stdint.h>
#include "interrupts/interrupts.h"
#include "interrupts/idt.h"
#include "interrupts/exceptions/init_exceptions.h"
#include <debug/include.h>
#include "interrupts/hardware/pic.h"

struct interrupt_frame;

__attribute__((interrupt)) void handle_keypress(struct interrupt_frame *frame)
{
    printf("Keypress\n");
}

void init_interrupts(InterruptDescriptor64 idt[256])
{
    uint16_t code_segment;

    asm("mov %%cs, %0" : "=r"(code_segment));

    uint16_t segment_selector = code_segment | 0x000; // GDT, privilege level 0

    init_exceptions(idt, segment_selector);

    // FIXME: Can't call interrupts above 15??
    // idt[15] = interrupts__new_entry(segment_selector, handle_keypress, TRAP_GATE, 0);
    lidt(idt, 255);

    // Pic__initialise();

    // asm("sti");
    // asm("int $15");
}
