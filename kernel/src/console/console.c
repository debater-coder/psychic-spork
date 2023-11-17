#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <stdbool.h>

#define SSFN_CONSOLEBITMAP_TRUECOLOR
#define SSFN_CONSOLEBITMAP_CONTROL
#include <ssfn.h>

#include "assets/font/font.h"
#include "console.h"
#include "interrupts/hardware/port.h"

void console__put_character(const char *msg, uint32_t fg)
{
    ssfn_dst.fg = fg;

    ssfn_putc(*msg);
}

void console__put_string(const char *msg, uint32_t fg)
{
    ssfn_dst.fg = fg;

    while (*msg)
    {
        ssfn_putc(*msg++);
    }
}

static struct limine_framebuffer *global_framebuffer;

void console__clear(uint32_t color)
{
    for (uint64_t i = 0; i < global_framebuffer->height; i++)
    {
        for (uint64_t j = 0; j < global_framebuffer->width; j++)
        {
            uint32_t *framebuffer_pointer = global_framebuffer->address;

            framebuffer_pointer[i * global_framebuffer->pitch / 4 + j] = color;
        }

        // Reset to top
        ssfn_dst.x = ssfn_dst.y = 0;
    }

    // Set new bg
    ssfn_dst.bg = 0xff000000 | color;
}

void console__init(struct limine_framebuffer *framebuffer)
{
    ssfn_src = &fontData;

    ssfn_dst.ptr = framebuffer->address;
    ssfn_dst.w = framebuffer->width;
    ssfn_dst.h = framebuffer->height;
    ssfn_dst.p = framebuffer->pitch; /* bytes per line */
    ssfn_dst.x = ssfn_dst.y = 0;     /* pen position */
    ssfn_dst.fg = 0xFFFFFF;          /* foreground color */
    ssfn_dst.bg = 0xff000000;        /* background color */

    global_framebuffer = framebuffer;
}
void console__input_character(char *input, uint32_t fg)
{
    char lowercase[] = "?\0001234567890-=\b\tqwertyuiop[]\0?asdfghjkl;'`?\\zxcvbnm,./??? ";

    uint8_t scan_code = port_read(0x60);
    while (scan_code >= 59)
    {
        scan_code = port_read(0x60);
    }

    if (lowercase[scan_code] > 1)
    {
        console__put_character(&lowercase[scan_code], fg);
    }
    *input = lowercase[scan_code];

    while (port_read(0x60) == scan_code)
    {
        // Wait until next key pressed or key released
    }
}

void console__input_characters(char *input, int count, uint32_t fg, bool null_terminate)
{
    for (int i = 0; i < count; i++)
    {
        console__input_character(&input[i], fg);
        if (input[i] == '\0')
        {
            return;
        }
    }
    if (null_terminate)
    {
        input[count] = '\0';
    }
}
