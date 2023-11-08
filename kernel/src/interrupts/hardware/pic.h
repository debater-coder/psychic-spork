#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct Pic
{
    uint8_t offset; // Base interrupt offset
    uint8_t command_port;
    uint8_t data_port;
} Pic;

bool Pic__handles_interrupt(Pic pic, uint8_t interrupt_id);
void Pic__end_of_interrupt(Pic pic);
uint8_t PIC__read_mask(Pic pic);
void PIC__write_mask(Pic pic, uint8_t mask);

typedef struct ChainedPics
{
    Pic pics[2];
} ChainedPics;

ChainedPics ChainedPics__new(uint8_t offset1, uint8_t offset2);
void ChainedPics__initialise(ChainedPics self);
void ChainedPics__notify_end_of_interrupt(ChainedPics self, uint8_t interrupt_id);