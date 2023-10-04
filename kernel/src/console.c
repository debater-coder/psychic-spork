#include "console.h"

#include "font.h"

#define SSFN_CONSOLEBITMAP_TRUECOLOR
#define SSFN_CONSOLEBITMAP_CONTROL
#include <ssfn.h>

void console_init(struct limine_framebuffer* framebuffer) {
    ssfn_src = &fontData;

    ssfn_dst.ptr = framebuffer->address;
    ssfn_dst.w = framebuffer->width;
    ssfn_dst.h = framebuffer->height;
    ssfn_dst.p = framebuffer->pitch;            /* bytes per line */
    ssfn_dst.x = ssfn_dst.y = 0;                /* pen position */
    ssfn_dst.fg = 0xFFFFFF;                     /* foreground color */
    ssfn_dst.bg = 0xFF000000;                   /* background color */
}

void console_puts(const char* str) {
    console_put_with_color(str, 0xFFFFFF);
}

void console_put_with_color(const char *str, uint32_t fg)
{
    ssfn_dst.fg = fg;

    while (*str) {
        ssfn_putc(*str++);
    }
}
