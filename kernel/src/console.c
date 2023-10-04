#include "console.h"

#include "font.h"

#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include <ssfn.h>

void console_init(struct limine_framebuffer* framebuffer) {
    ssfn_src = &fontData;

    ssfn_dst.ptr = framebuffer->address;
    ssfn_dst.w = framebuffer->width;
    ssfn_dst.h = framebuffer->height;
    ssfn_dst.p = framebuffer->pitch;                          /* bytes per line */
    ssfn_dst.x = ssfn_dst.y = 0;                /* pen position */
    ssfn_dst.fg = 0xFFFFFF;                     /* foreground color */
}

void console_puts(const char* str) {
    while (*str) {
        // Handle control strings
        if (*str == '\n') {
            ssfn_dst.x = 0;
            ssfn_dst.y += ssfn_src->height;
            str++;
            continue;
        }

        ssfn_putc(*str++);
    }
}