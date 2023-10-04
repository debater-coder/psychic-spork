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
    console_put_with_color(str, 0xFFFFFF, 0x000000);
}

void console_put_with_color(const char *str, uint32_t fg, uint32_t bg)
{
    ssfn_dst.fg = fg;
    ssfn_dst.bg = bg;

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
