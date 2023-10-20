#pragma once
#include <stdint.h>

typedef struct TtyStyle
{
    uint32_t fg;
    uint32_t bg;
} TtyStyle;

typedef struct TtyContext
{
    void (*put_string)(const char *, const TtyStyle *);
    void (*put_character)(const char *, const TtyStyle *);
    void (*clear)(unsigned int);
} TtyContext;