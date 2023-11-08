#pragma once
#include <stdint.h>
#include <stdbool.h>

void Pic__initialise();
void Pic__notify_end_of_interrupt(uint8_t interrupt_id);