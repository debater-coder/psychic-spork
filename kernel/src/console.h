#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limine.h>

void console_init(struct limine_framebuffer* framebuffer);

void console_puts(const char *str);
void console_printf(const char *fmt, ...);

void console_put_with_color(const char *str, uint32_t fg);