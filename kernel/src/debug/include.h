// debug/include.h
// Utils for printing and debugging
#pragma once
#include <stdint.h>

void exit();
void to_hex_string(unsigned long long int num, char *result);
void init_debug(uint32_t fg);
void printf(char *fmt, ...);
void panic(char *msg);
uint64_t from_hex_string(char *input);