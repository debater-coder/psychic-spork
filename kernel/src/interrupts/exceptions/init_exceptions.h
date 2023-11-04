#pragma once
#include "interrupts/idt.h"

void init_exceptions(InterruptDescriptor64 idt[256], uint16_t segment_selector);