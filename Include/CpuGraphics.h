#ifndef DUMPGRAPHICS_H
#define DUMPGRAPHICS_H

#include <stdio.h>
#include <cstring>
#include <stdarg.h>
#include "../Libs/Stack/Include/ColourConsts.h"
#include "./Constants.h"
#include "../Libs/Stack/Include/Stack.h"

#ifndef  NDUMP

#define SimpleStackDump( stack_ptr )                 \
{                                                    \
    SimpleStackDump_(stack_ptr);                     \
    fprintf(stderr, "\n");                           \
}

#else

#define SimpleStackDump( stack_ptr ) {}

#endif

void WriteNSymb(size_t n, char symb)
{
    for (int i = 0; i < n; i++)
        fprintf(stderr, "%c", symb);
}

#ifdef DELAYS_MODE

void PrintLoading(float delay, const char* fmt_msg, ...)
{
    ASSERT(fmt_msg != NULL);

    va_list args;
    va_start(args, fmt_msg);

    vfprintf(stderr, fmt_msg, args);

    va_end(args);

    int n_cycles = (int) (delay / MIN_CYCLE_DELAY);

    struct timespec t_r = {0, (int) (MIN_CYCLE_DELAY / 2 / 3 * 1000000000)};
    struct timespec t_w;

    for (int i = 0; i < n_cycles; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            nanosleep(&t_r, &t_w);
            fprintf(stderr, ".");
            nanosleep(&t_r, &t_w);
        }
        fprintf(stderr, "\b\b\b   \b\b\b");
    }
    fprintf(stderr, "\r                                                     \r");
}

#else

void PrintLoading(float delay, const char* fmt_msg, ...) {}

#endif

int PrintRusFlag()
{
    fprintf(stderr, "\n");
    fprintf(stderr, KWHT "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n" KNRM);
    fprintf(stderr, KBLU "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n" KNRM);
    fprintf(stderr, KRED "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n"
                         "  RUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIARUSSIA\n" KNRM);
    fprintf(stderr, "\n");

    return 0xDED;
}

#endif
