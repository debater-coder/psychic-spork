#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "arch/include.h"
#include "memory/necessities.h"
#include "debug/include.h"
#include "drivers/limine_framebuffer/include.h"

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
void _start(void)
{
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        arch__exit();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    TtyContext tty_ctx = drivers__limine_framebuffer__init(framebuffer);
    init_debug(&tty_ctx, &(TtyStyle){.bg = 0, .fg = 0xffffff});

    if (memmap_request.response == NULL)
    {
        panic("No memory map!");
    }

    uint64_t usable_memory = 0;

    printf("memory areas:\n");
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        if ((*memmap_request.response->entries)[i].type == LIMINE_MEMMAP_USABLE)
        {
            printf("    start: 0x%x, length: 0x%x\n",
                   (*memmap_request.response->entries)[i].base,
                   (*memmap_request.response->entries)[i].length);

            usable_memory += (*memmap_request.response->entries)[i].length;
        }
    }
    printf("Usable memory: %x GiB\n", usable_memory / 1073741824ULL);

    printf("It did not fail!\n");

    // We're done, just hang...
    arch__exit();
}
