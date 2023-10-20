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

static struct limine_framebuffer *global_framebuffer;

void clear(unsigned int color)
{
    for (int i = 0; i < global_framebuffer->height; i++)
    {
        for (int j = 0; j < global_framebuffer->width; j++)
        {
            uint32_t *framebuffer_pointer = global_framebuffer->address;

            framebuffer_pointer[i * global_framebuffer->pitch / 4 + j] = color;
        }

        // Reset to top
        ssfn_dst.x = ssfn_dst.y = 0;
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
    ssfn_dst.bg = 0;                 /* background color */

    global_framebuffer = framebuffer;

    return (TtyContext){
        .put_string = &put_string, .put_character = &put_character, .clear = &clear};
}