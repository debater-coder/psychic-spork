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

struct interrupt_frame;

__attribute__((interrupt)) void handle_div_zero(struct interrupt_frame *frame)
{
    panic("EXCEPTION_DIV_ZERO");
}

__attribute__((interrupt)) void handle_page_fault(struct interrupt_frame *frame, uint64_t error_code)
{
    panic("EXCEPTION_PAGE_FAULT");
}
__attribute__((interrupt)) void
handle_double_fault(struct interrupt_frame *frame, uint64_t error_code)
{
    panic("EXCEPTION_DOUBLE_FAULT");
}

void init_interrupts()
{
    uint16_t code_segment;

    asm("mov %%cs, %0" : "=r"(code_segment));

    uint16_t segment_selector = code_segment | 0x000; // GDT, privilege level 0

    idt[0] = interrupts__new_entry(segment_selector, handle_div_zero, TRAP_GATE, 0);
    idt[8] = interrupts__new_entry(segment_selector, handle_double_fault, TRAP_GATE, 0);
    idt[14] = interrupts__new_entry(segment_selector, handle_page_fault, TRAP_GATE, 0);

    lidt(idt, 255);
}

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
        arch__exit();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    TtyContext tty_ctx = drivers__limine_framebuffer__init(framebuffer);
    init_debug(&tty_ctx, &(TtyStyle){.bg = 0, .fg = 0xffffff});

    init_interrupts();

    divide_by_zero();
    cause_page_fault();

    printf("It did not fail!\n");

    // We're done, just hang...
    arch__exit();
}
