#ifndef DUMPGRAPHICS_H
#define DUMPGRAPHICS_H

#include <stdio.h>
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


void FullDump(int* code, int code_size, int ip, Stack* stack)
{
    fprintf(stderr, "  \\\\=========");

    for(int i = 0; i < code_size; i++)
        fprintf(stderr, "===");

    fprintf(stderr, "\\\\\n    ip:   ");

    for (int i = 0; i < code_size; i++)
    {
        fprintf(stderr, "%02d ", i);
    }

    fprintf(stderr, "\n    code: ");

    for (int i = 0; i < code_size; i++)
    {
        fprintf(stderr, "%02d ", code[i]);
    }

    fprintf(stderr, "\n    ------");

    for(int i = 0; i < ip; i++)
        fprintf(stderr, "---");

    fprintf(stderr, "^ ip = %d\n", ip);

    SimpleStackDump_(stack);

    fprintf(stderr, "  \\\\=========");

    for(int i = 0; i < code_size; i++)
        fprintf(stderr, "===");

    fprintf(stderr, "\\\\\n\n");
}

#endif
