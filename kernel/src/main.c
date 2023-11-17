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

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
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

void strcpy(char *dst, char *src)
{
    while (*src)
    {
        *dst = *src;
        src++;
        dst++;
    }
    *dst = *src;
}

void kernel_main()
{
    printf("BenchOS 0.1.0\n");
    printf("Using framebuffer at 0x%x\n", framebuffer_request.response->framebuffers[0]->address);
    printf("Offset 0x%x\n", hhdm_request.response->offset);

    printf("Detecting memory...\n");
    if (memmap_request.response == NULL)
    {
        panic("NO_MEMORY_MAP");
    }

    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        uint64_t type = memmap_request.response->entries[i]->type;
        char memmap_type[256];
        switch (type)
        {
        case LIMINE_MEMMAP_USABLE:
            strcpy(memmap_type, "USABLE");
            break;

        case LIMINE_MEMMAP_RESERVED:
            strcpy(memmap_type, "RESERVED");
            break;

        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            strcpy(memmap_type, "ACPI_RECLAIMABLE");
            break;

        case LIMINE_MEMMAP_ACPI_NVS:
            strcpy(memmap_type, "ACPI_NVS");
            break;

        case LIMINE_MEMMAP_BAD_MEMORY:
            strcpy(memmap_type, "BAD");
            break;

        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            strcpy(memmap_type, "BOOTLOADER_RECLAIMABLE");
            break;

        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            strcpy(memmap_type, "KERNEL_AND_MODULES");
            break;

        case LIMINE_MEMMAP_FRAMEBUFFER:
            strcpy(memmap_type, "FRAMEBUFFER");
            break;

        default:
            strcpy(memmap_type, "UNKNOWN");
            break;
        }
        uint64_t phys = memmap_request.response->entries[i]->base;
        uint64_t virt = hhdm_request.response->offset + phys;
        printf("    base: 0x%x, length: 0x%x, status: %s\n", virt, memmap_request.response->entries[i]->length, memmap_type);
    }

    for (;;)
    {
        char input[257];
        printf("Input up to 256 characters (ESC to stop): ");
        console__input_characters(input, 256, 0xffff00, true);
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

    if (hhdm_request.response == NULL)
    {
        panic("NO_HHDM");
    }

    init_interrupts(idt);

    kernel_main();
    exit();
}