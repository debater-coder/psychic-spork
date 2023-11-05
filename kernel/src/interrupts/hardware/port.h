#pragma once
#include <stdint.h>

inline void port_write(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

inline uint8_t port_read(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port)
                 : "memory");
    return ret;
}