#pragma once
#include <stdint.h>
#include "interrupts/idt.h"

void init_interrupts(InterruptDescriptor64 idt[256]);