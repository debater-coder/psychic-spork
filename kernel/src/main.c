#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "arch/include.h"
#include "memory/necessities.h"
#include "debug/include.h"
#include "drivers/limine_framebuffer/include.h"

static inline void lidt(void *base, uint16_t size)
{ // This function works in 32 and 64bit mode
    struct
    {
        uint16_t length;
        void *base;
    } __attribute__((packed)) IDTR = {size, base};

    asm("lidt %0" : : "m"(IDTR)); // let the compiler choose an addressing mode
}

static InterruptDescriptor64 idt[256];

void handle_double_fault()
{
    panic("EXCEPTION_DOUBLE_FAULT");
}

void init_interrupts()
{
    uint16_t code_segment;

    asm("mov %%cs, %0" : "=r"(code_segment));

    uint16_t segment_selector = code_segment | 0x000; // GDT, privilege level 0

    idt[8] = interrupts__new_entry(segment_selector, handle_double_fault, TRAP_GATE, 0);
    lidt(idt, 255);
}

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
void _start()
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

    init_interrupts();

    int *ptr = (void *)0;
    *ptr = 5;

    printf("It did not fail!\n");

    // We're done, just hang...
    arch__exit();
}
