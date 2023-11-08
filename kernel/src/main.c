#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "memory/necessities.h"
#include "debug/include.h"
#include "console/console.h"
#include "interrupts/interrupts.h"
#include "interrupts/hardware/port.h"

static InterruptDescriptor64 idt[256];

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

void kernel_main()
{

    printf("BenchOS 0.1.0\n");
    for (;;)
    {
        char input[9];
        printf("Input 8 characters: ");
        console__input_characters(input, 8, 0xffff00, true);
        printf("\nYou wrote: %s\n", input);
    }
}

void _start()
{
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        panic("NO_FRAMEBUFFER"); // Panic function before init_debug will just exit
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    console__init(framebuffer);
    init_debug(0xffffff);

    init_interrupts(idt);

    kernel_main();
    exit();
}