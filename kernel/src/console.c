#include "console.h"

#include "font.h"
#include <stdarg.h>

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

void console_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    ssfn_dst.fg = 0xFFFFFF;

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's': {
                    char *str = va_arg(args, char*);
                    while (*str) {
                        ssfn_putc(*str++);
                    }
                    break;
                }
                case 'c': {
                    char c = va_arg(args, int);
                    ssfn_putc(c);
                    break;
                }
                case 'd': {
                    int i = va_arg(args, int);
                    char integer[16] = "";
                    citoa(i, integer, 10);
                    console_puts(integer);
                    break;
                }
                case 'x': {
                    int i = va_arg(args, int);
                    char integer[16] = "";
                    citoa(i, integer, 16);
                    console_puts(integer);
                    break;
                }
                case 'b': {
                    int i = va_arg(args, int);
                    char integer[16] = "";
                    citoa(i, integer, 2);
                    console_puts(integer);
                    break;
                }
                case '%': {
                    ssfn_putc('%');
                    break;
                }
                default: {
                    ssfn_putc('%');
                    ssfn_putc(*fmt);
                    break;
                }
            }
            fmt++;
        }

        ssfn_putc(*fmt++);
    }

    va_end(args);
}
