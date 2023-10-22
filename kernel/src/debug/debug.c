#include "driver_interfaces/tty/include.h"
#include "arch/include.h"
#include <stdbool.h>
#include <stdarg.h>

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

static TtyContext *tty_ctx;
static TtyStyle *tty_style;
static bool init = false;

void init_debug(TtyContext *tty, TtyStyle *style)
{
    tty_ctx = tty;
    tty_style = style;
    init = true;
}

void panic(char *msg)
{
    if (init)
    {
        tty_ctx->clear(0xff38a169);

        tty_ctx->put_string("Kernel Panic\n============\n\nYour computer has run into an irrecoverable issue and needs to restart. To\nrestart, hold down the power button and you'll be back on track in no time.\nIf the problem persists, note down the error code, fork the repository and fix\nit yourself.\n\nError code: ", &(TtyStyle){.fg = 0, .bg = 0});

        tty_ctx->put_string(
            msg, &(TtyStyle){.fg = 0xff5555, .bg = 0});
    }

    arch__exit();
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
                tty_ctx->put_string(va_arg(fmt_args, char *), tty_style);
                break;
            case 'x':
                unsigned long long int num = va_arg(fmt_args, unsigned long long int);
                char num_hex[16];
                to_hex_string(num, num_hex);
                tty_ctx->put_string(num_hex, tty_style);
                break;
            default:
                tty_ctx->put_character('%', tty_style);
                tty_ctx->put_character(*fmt, tty_style);
                break;
            }
        }
        else
        {
            tty_ctx->put_character(fmt, tty_style);
        }

        fmt++;
    }
}