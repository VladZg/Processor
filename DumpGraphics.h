#ifndef DUMPGRAPHICS_H
#define DUMPGRAPHICS_H

#include <stdio.h>
#include "Stack/ColourConsts.h"
#include "Stack/Stack.h"

#ifndef  NDUMP

#define SimpleStackDump( stack_ptr, cmd )            \
{                                                    \
    fprintf(stderr, "  %s:\n", cmd);                 \
    SimpleStackDump_(stack_ptr);                     \
    fprintf(stderr, "\n");                           \
}

#else

#define SimpleStackDump( stack_ptr, cmd ) {}

#endif

void WriteNSymb(size_t n, char symb)
{
    for (int i = 0; i < n; i++)
        fprintf(stderr, "%c", symb);
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
