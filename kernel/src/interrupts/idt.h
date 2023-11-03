#pragma once
#include <stdint.h>

#define INTERRUPT_GATE 0x8E
#define TRAP_GATE 0x8F

typedef struct __attribute__((__packed__)) InterruptDescriptor64
{
    uint16_t handler_pointer_low;  // offset bits 0..15
    uint16_t segment_selector;     // a code segment selector in GDT or LDT
    uint8_t ist;                   // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t type_attributes;       // gate type, dpl, and p fields
    uint16_t handler_pointer_med;  // offset bits 16..31
    uint32_t handler_pointer_high; // offset bits 32..63
    uint32_t zero;                 // reserved
} InterruptDescriptor64;

InterruptDescriptor64 interrupts__new_entry(uint16_t segment_selector, void (*handler)(), uint8_t type_attributes, uint8_t ist);
void lidt(void *base, uint16_t size);