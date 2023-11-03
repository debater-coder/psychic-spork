#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
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

void cause_page_fault()
{
    int x = *(int *)(0);
    printf("%x", x);
}

void divide_by_zero()
{
    asm("movl $0, %%edx; divl %%edx" ::: "ax", "dx");
}

void _start()
{
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        exit(); // We are so early in the OS that the panic function is not available yet
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    TtyContext tty_ctx = drivers__limine_framebuffer__init(framebuffer);
    init_debug(&tty_ctx, &(TtyStyle){.bg = 0, .fg = 0xffffff});

    init_interrupts();

    panic("AOLIN'S SKILL ISSUE");

    printf("It did not fail!\n");

    // We're done, just hang...
    exit();
}
