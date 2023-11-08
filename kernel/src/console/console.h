#pragma once
#include <stdint.h>
#include <limine.h>

void console__put_character(const char *msg, uint32_t fg);
void console__put_string(const char *msg, uint32_t fg);
void console__clear(unsigned int color);
void console__init(struct limine_framebuffer *framebuffer);
void console__input_character(char *input, uint32_t fg);