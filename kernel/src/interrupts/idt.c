#include "interrupts/idt.h"

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

void lidt(void *base, uint16_t size)
{
    struct
    {
        uint16_t length;
        void *base;
    } __attribute__((packed)) IDTR = {size, base};

    asm("lidt %0" : : "m"(IDTR)); // let the compiler choose an addressing mode
}