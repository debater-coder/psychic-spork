// drivers/limine_framebuffer/include.h
// Provides a `TtyContext` for a limine_framebuffer TODO: don't hog the global
// ssfn thing, we want to be able to have multiple of these eventually...

#pragma once
#include <limine.h>
#include "driver_interfaces/tty/include.h"


TtyContext drivers__limine_framebuffer__init(struct limine_framebuffer *framebuffer);