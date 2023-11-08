#include <stdbool.h>
#include <stdint.h>
#include "interrupts/hardware/pic.h"
#include "interrupts/hardware/port.h"
#include "pic.h"

#define CMD_INIT 0x11
#define CMD_END_OF_INTERRUPT 0x20
#define MODE_8086 0x01
#define PIC_1_COMMAND 0x20
#define PIC_1_DATA 0x21
#define PIC_2_COMMAND 0xa0
#define PIC_2_DATA 0xa1
#define PIC_EOI 0x20

static void wait()
{
    port_write(0x80, 0);
}

void Pic__initialise()
{
    // Tell both PICs we are doing init
    port_write(PIC_1_COMMAND, CMD_INIT);
    wait();
    port_write(PIC_2_COMMAND, CMD_INIT);
    wait();

    // Set up offsets
    port_write(PIC_1_DATA, 0x20);
    wait();
    port_write(PIC_2_DATA, 0x28);
    wait();

    // Chain Pics
    port_write(PIC_1_DATA, 4);
    wait();
    port_write(PIC_2_DATA, 2);
    wait();

    // Set mode
    port_write(PIC_1_DATA, MODE_8086);
    wait();
    port_write(PIC_1_DATA, MODE_8086);
    wait();

    // Init is done now let's do masks:
    // Set correct masks:
    port_write(PIC_1_DATA, 0xfd); // 11111101 (only keyboard)
    wait();
    port_write(PIC_2_DATA, 0xff); // 11111111
    wait();
}

void Pic__notify_end_of_interrupt(uint8_t interrupt_id)
{
    if (interrupt_id >= 8)
    {
        port_write(PIC_2_COMMAND, PIC_EOI);
    }
    port_write(PIC_1_COMMAND, PIC_EOI);
}
