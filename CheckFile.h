#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <stdio.h>

size_t CheckFile(const char* filename)
{
    FILE* file = fopen(filename, "rb+");

    if (file)
    {
        fclose(file);
        return 1;
    }

    return 0;
}

#endif
