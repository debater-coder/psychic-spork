#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "memory.h"
#include "console.h"
#include "utils.h"

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

// Halt and catch fire function.
static void
hcf()
{
    asm("cli");
    for (;;)
    {
        asm("hlt");
    }
}

static void panic(char *message)
{
    console_put_with_color(message, 0xff8888);
    hcf();
}

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
void _start(void)
{
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    console_init(framebuffer); // initialise the console

    if (memmap_request.response == NULL)
    {
        panic("No memory map!");
    }

    console_puts("memory areas:\n");
    for (int i = 0; i < memmap_request.response->entry_count; i++)
    {
        if ((*memmap_request.response->entries)[i].type == LIMINE_MEMMAP_USABLE)
        {
            char start[16];

            console_puts("   start: 0x");
            to_hex_string((*memmap_request.response->entries)[i].base, start);
            console_puts(start);
            console_puts("\n");

            char length[16];

            console_puts("   length: 0x");
            to_hex_string((*memmap_request.response->entries)[i].length, length);
            console_puts(length);
            console_puts("\n\n");
        }
    }

    console_puts("It did not fail!");

    // We're done, just hang...
    hcf();
}
