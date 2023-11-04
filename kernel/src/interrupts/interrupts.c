#include <stdint.h>
#include "interrupts/interrupts.h"
#include "interrupts/idt.h"
#include "interrupts/exceptions/init_exceptions.h"

static InterruptDescriptor64 idt[256];

void init_interrupts()
{
    uint16_t code_segment;

    asm("mov %%cs, %0" : "=r"(code_segment));

    uint16_t segment_selector = code_segment | 0x000; // GDT, privilege level 0

    init_exceptions(idt, segment_selector);

    lidt(idt, 255);
}
