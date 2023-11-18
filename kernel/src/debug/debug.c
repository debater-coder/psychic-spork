#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <console/console.h>
#include "include.h"

void exit()
{
    asm("cli"); // If we don't clear interrupts the CPU could un-halt
    for (;;)    // Just in case we get un-halted the CPU will immediately halt in the next iteration of the loop
    {
        asm("hlt");
    }
}

unsigned long long int log2(unsigned long long int n)
{
    unsigned long long int val;
    for (val = 0; n > 1; val++, n >>= 1)
        ;
    return val;
}

static char hex_digits[] = "0123456789abcdef";

void to_hex_string(unsigned long long int num, char *result)
{
    if (num == 0)
    {
        *result = '0';
        result++;
        *result = '\0';
    }

    int bits = log2(num);
    int length = (bits + 4) / 4;

    // TODO: malloc length instead of needing input string

    for (int i = length - 1; i >= 0; i--)
    {
        result[i] = hex_digits[num % 16];
        num /= 16;
    }

    result[length] = '\0';
}

static uint32_t global_fg = 0xffffff;
static bool init = false;

void init_debug(uint32_t fg)
{
    global_fg = fg;
    init = true;
}

void panic(char *msg)
{
    if (init)
    {
        console__clear(0xff38a169);

        console__put_string("Kernel Panic\n============\n\nYour computer has run into an irrecoverable issue and needs to restart. To\nrestart, hold down the power button and you'll be back on track in no time.\nIf the problem persists, note down the error code, fork the repository and fix\nit yourself.\n\nError code: ", 0);

        console__put_string(msg, 0xff5555);
    }

    exit();
}

static uint32_t strlen(char *str)
{
    uint32_t l = 0;
    while (*str)
    {
        l++;
        str++;
    }
}

uint64_t from_hex_string(char *input)
{
    uint32_t length = strlen(input);
    printf("\nlength: 0x%x\n", length);
    uint64_t num = 0;
    while (*input)
    {
        int digit;
        switch (*input)
        {
        case '0':
            digit = 0;
            break;
        case '1':
            digit = 1;
            break;
        case '2':
            digit = 2;
            break;
        case '3':
            digit = 3;
            break;
        case '4':
            digit = 4;
            break;
        case '5':
            digit = 5;
            break;
        case '6':
            digit = 6;
            break;
        case '7':
            digit = 7;
            break;
        case '8':
            digit = 8;
            break;
        case '9':
            digit = 9;
            break;
        case 'a':
            digit = 10;
            break;
        case 'b':
            digit = 11;
            break;
        case 'c':
            digit = 12;
            break;
        case 'd':
            digit = 13;
            break;
        case 'e':
            digit = 14;
            break;
        case 'f':
            digit = 15;
            break;
        }
        num += digit * length * 16;
        input++;
        length--;
    }
    return num;
}

void printf(char *fmt, ...)
{
    va_list fmt_args;
    va_start(fmt_args, fmt);

    while (*fmt)
    {

        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 's':
                console__put_string(va_arg(fmt_args, char *), global_fg);
                break;
            case 'x':
                unsigned long long int num = va_arg(fmt_args, unsigned long long int);
                char num_hex[17];
                to_hex_string(num, num_hex);
                console__put_string(num_hex, global_fg);
                break;
            default:
                console__put_character("%", global_fg);
                console__put_character(fmt, global_fg);
                break;
            }
        }
        else
        {
            console__put_character(fmt, global_fg);
        }

        fmt++;
    }
}