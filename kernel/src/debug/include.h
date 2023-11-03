// debug/include.h
// Utils for printing and debugging
#pragma once
#include "driver_interfaces/tty/include.h"

void exit();
void to_hex_string(unsigned long long int num, char *result);
void init_debug(TtyContext *tty, TtyStyle *style);
void printf(char *fmt, ...);
void panic(char *msg);