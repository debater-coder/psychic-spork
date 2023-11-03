#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "memory/necessities.h"
#include "debug/include.h"
#include "console/console.h"

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

    console__init(framebuffer);
    init_debug(0xffffff);

    init_interrupts();

    divide_by_zero();

    printf("It did not fail!\n");

    // We're done, just hang...
    exit();
}
