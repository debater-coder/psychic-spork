#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <console/console.h>

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
                char num_hex[16];
                to_hex_string(num, num_hex);
                console__put_string(num_hex, global_fg);
                break;
            default:
                console__put_character('%', global_fg);
                console__put_character(*fmt, global_fg);
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