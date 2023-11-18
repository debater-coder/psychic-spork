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

bool stringeq(char *a, char *b)
{
    while (*a || *b)
    {
        if (*a != *b)
        {
            return false;
        }
        a++;
        b++;
    }
    return true;
}

typedef struct FrameAllocator
{
    uint64_t physical_offset;
} FrameAllocator;

uint64_t memory__frame_allocator_phys_to_virt(FrameAllocator *self, uint64_t address)
{
    return self->physical_offset + address;
};

void kernel_main()
{
    printf("BenchOS 0.1.0\n");
    printf("Using framebuffer at 0x%x\n", framebuffer_request.response->framebuffers[0]->address);
    FrameAllocator frame_allocator;
    frame_allocator.physical_offset = hhdm_request.response->offset;
    printf("Offset 0x%x\n", frame_allocator.physical_offset);

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
        printf(
            "    phys: 0x%x, virt: 0x%x, length: 0x%x, status: %s\n",
            phys,
            memory__frame_allocator_phys_to_virt(&frame_allocator, phys),
            memmap_request.response->entries[i]->length,
            memmap_type);
    }

    printf("\n\nThis is the prompt. Type `help` for more info.\n\n");

    for (;;)
    {
        char input[257];

        printf("> ");
        console__input_characters(input, 256, 0xffff00, true);
        printf("\n");

        if (!*input)
        {
        }
        else if (stringeq(input, "exit"))
        {
            printf("exiting...\n");
            exit();
        }
        else if (stringeq(input, "help"))
        {
            printf("BenchOS v0.1.0\n\
Commands:\n\
    - help\n\    
    - exit\n\
    - clear\n");
        }
        else if (stringeq(input, "clear"))
        {
            console__clear(0);
        }
        else if (stringeq(input, "dumppage"))
        {
            printf("Enter address: ");
            console__input_characters(input, 16, 0xffff00, true);
            printf("\n0x%x\n", from_hex_string(input));
        }
        else
        {
            console__put_string("Unknown command: ", 0xff0000);
            console__put_string(input, 0xff0000);
            console__put_string("\n", 0xff0000);
        }
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