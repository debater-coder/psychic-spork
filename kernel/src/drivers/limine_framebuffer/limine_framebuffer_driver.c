#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#define SSFN_CONSOLEBITMAP_TRUECOLOR
#define SSFN_CONSOLEBITMAP_CONTROL
#include <ssfn.h>

#include "assets/font/font.h"
#include "driver_interfaces/tty/include.h"

void put_character(const char *msg, const TtyStyle *style)
{
    ssfn_dst.fg = style->fg;

    ssfn_putc(*msg);
}

void put_string(const char *msg, const TtyStyle *style)
{
    ssfn_dst.fg = style->fg;

    while (*msg)
    {
        ssfn_putc(*msg++);
    }
}

TtyContext drivers__limine_framebuffer__init(struct limine_framebuffer *framebuffer)
{
    ssfn_src = &fontData;

    ssfn_dst.ptr = framebuffer->address;
    ssfn_dst.w = framebuffer->width;
    ssfn_dst.h = framebuffer->height;
    ssfn_dst.p = framebuffer->pitch; /* bytes per line */
    ssfn_dst.x = ssfn_dst.y = 0;     /* pen position */
    ssfn_dst.fg = 0xFFFFFF;          /* foreground color */
    ssfn_dst.bg = 0xFF000000;        /* background color */

    return (TtyContext){
        .put_string = &put_string, .put_character = &put_character};
}