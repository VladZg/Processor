#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <stdio.h>

size_t IsFileExist(const char* filename)
{
    FILE* file = fopen(filename, "rb+");

    if (file)
    {
        fclose(file);
        return 1;
    }

    return 0;
}

size_t CheckFile(const int argc, const char** argv, const char** filename_input)
{
    if (argc == 2)
    {
        if (IsFileExist(argv[1]))
        {
            *filename_input = argv[1];

            return 1;
        }

        else
        {
            fprintf(stderr, "  NO FILE \"%s\" IN THIS DIRECTORY\n\n", argv[1]);

            exit(1);
        }
    }

    else return 0;
}

#endif
