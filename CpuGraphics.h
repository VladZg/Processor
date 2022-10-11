#ifndef DUMPGRAPHICS_H
#define DUMPGRAPHICS_H

#include <stdio.h>
#include <cstring>
#include "Stack/ColourConsts.h"
#include "Constants.h"
#include "Stack/Stack.h"

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

void PrintLoading(float delay)
{
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
