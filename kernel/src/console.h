#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limine.h>

void console_init(struct limine_framebuffer* framebuffer);

void console_puts(const char *str);