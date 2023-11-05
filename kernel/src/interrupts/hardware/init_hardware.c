#include "init_hardware.h"
#include "debug/include.h"
#include <stdint.h>
#include <stdbool.h>
#include "interrupts/hardware/pic.h"

void init_hardware()
{
    ChainedPics pics = ChainedPics__new(32, 40);
    ChainedPics__initialise(pics);
}