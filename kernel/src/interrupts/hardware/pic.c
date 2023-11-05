#include <stdbool.h>
#include <stdint.h>
#include "interrupts/hardware/pic.h"
#include "interrupts/hardware/port.h"
#include "pic.h"

#define CMD_INIT 0x11
#define CMD_END_OF_INTERRUPT 0x20
#define MODE_8086 0x01

// Implementation based on: https://docs.rs/crate/pic8259/0.10.1/source/src/lib.rs

bool Pic__handles_interrupt(Pic pic, uint8_t interrupt_id)
{
    return pic.offset <= interrupt_id && interrupt_id < pic.offset + 8;
}

void Pic__end_of_interrupt(Pic pic)
{
    port_write(pic.command_port, CMD_END_OF_INTERRUPT);
}

uint8_t PIC__read_mask(Pic pic)
{
    return port_read(pic.data_port);
}

void PIC__write_mask(Pic pic, uint8_t mask)
{
    port_write(pic.data_port, mask);
}

ChainedPics ChainedPics__new(uint8_t offset1, uint8_t offset2)
{
    return (ChainedPics){
        .pics = {
            (Pic){
                .offset = offset1,
                .command_port = 0x20,
                .data_port = 0x21},
            (Pic){
                .offset = offset2,
                .command_port = 0xa0,
                .data_port = 0xa1}},
    };
}

static void wait()
{
    port_write(0x80, 0);
}

void ChainedPics__initialise(ChainedPics self)
{
    // Tell both PICs we are doing 3 byte init
    port_write(self.pics[0].command_port, CMD_INIT);
    wait();
    port_write(self.pics[1].command_port, CMD_INIT);
    wait();

    // Set up offsets
    port_write(self.pics[0].data_port, self.pics[0].offset);
    wait();
    port_write(self.pics[1].data_port, self.pics[1].offset);
    wait();

    // Chain Pics
    port_write(self.pics[0].data_port, 4);
    wait();
    port_write(self.pics[1].data_port, 2);
    wait();

    // Set mode
    port_write(self.pics[0].data_port, MODE_8086);
    wait();
    port_write(self.pics[1].data_port, MODE_8086);
    wait();

    // Init is done now let's do masks:
    // Set correct masks:
    port_write(self.pics[0].data_port, 0xfd); // 11111101
    port_write(self.pics[1].data_port, 0xff); // 11111111
}

void ChainedPics__notify_end_of_interrupt(ChainedPics self, uint8_t interrupt_id)
{
    if (interrupt_id >= 8)
        Pic__end_of_interrupt(self.pics[1]);

    Pic__end_of_interrupt(self.pics[0]);
}
