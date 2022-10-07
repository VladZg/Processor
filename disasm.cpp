#include "Constants.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "TechInfo.h"
#include "CheckFile.h"

#define  ASSERT( condition )                                                \
{                                                                           \
    if (!(condition))                                                       \
        fprintf(stderr, "Error in '%s' in line %d in file \"%s\" in %s\n",  \
                #condition, __LINE__, __FILE__, __PRETTY_FUNCTION__);       \
}

#undef FILENAME_INPUT
#undef FILENAME_OUTPUT
#undef FILENAME_INPUT_DEFAULT

const char  FILENAME_INPUT_DEFAULT[]  = "Source_output.asm";
const char* FILENAME_INPUT            = nullptr;
const char  FILENAME_OUTPUT[]         = "Source_output_disasmed.txt";

int main(int argc, char** argv)
{
    if (!CheckFile(argc, argv, &FILENAME_INPUT))
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;

    FILE* file_inp = fopen(FILENAME_INPUT, "rb");
    ASSERT(file_inp != NULL);

    TechInfo tech_info = {};
    fread(&tech_info, sizeof(int), TECH_INFO_SIZE, file_inp);

    if ((tech_info.filecode == CP_FILECODE) && (tech_info.version == CMD_VERSION))
    {
        int code_size = tech_info.code_size;
        char* code = (char*) calloc(code_size, sizeof(char));
        ASSERT(code != NULL);

        fread(code, sizeof(char), code_size, file_inp);

        FILE* file_out = fopen(FILENAME_OUTPUT, "w");
        ASSERT(file_out != NULL);

        int ip = 0;

        while (ip < code_size)
        {
            switch(code[ip])
            {
                case CMD_PUSH:
                {
                    ip++;
                    fprintf(file_out, "Push %d\n", *(int*)(code + ip));
                    ip += sizeof(int);

                    break;
                }

                case CMD_POP:
                {
                    fprintf(file_out, "Pop\n");
                    ip++;

                    break;
                }

                case CMD_ADD:
                {
                    fprintf(file_out, "Add\n");
                    ip++;

                    break;
                }

                case CMD_SUB:
                {
                    fprintf(file_out, "Sub\n");
                    ip++;

                    break;
                }

                case CMD_MUL:
                {
                    fprintf(file_out, "Mul\n");
                    ip++;

                    break;
                }

                case CMD_DIV:
                {
                    fprintf(file_out, "Div\n");
                    ip++;

                    break;
                }

                case CMD_OUT:
                {
                    fprintf(file_out, "Out\n");
                    ip++;

                    break;
                }

                case CMD_PIN:
                {
                    fprintf(file_out, "Pin\n");
                    ip++;

                    break;
                }

                case CMD_HLT:
                {
                    fprintf(file_out, "Hlt\n");
                    ip++;

                    break;
                }

                case CMD_DUMP:
                {
                    fprintf(file_out, "Dump\n");
                    ip++;

                    break;
                }
            }
        }

        fclose(file_inp);
        fclose(file_out);
    }

    else if (tech_info.filecode != CP_FILECODE)
    {
        fprintf(stderr, "    WRONG TYPE OF ASM FILE!!!\n"
                        "    YOU HAVE TO USE CP_FILECODE \"%d\"\n\n", CP_FILECODE);
        fclose(file_inp);
        exit(1);
    }

    else if (tech_info.version != CMD_VERSION)
    {
        fprintf(stderr, "    USED OLD (\"%d\") VERSION OF COMMANDS!!!\n"
                        "    YOU HAVE TO USE THE \"%d\" VERSION!!!\n\n",
                tech_info.version, CMD_VERSION);
        fclose(file_inp);
        exit(1);
    }

    return 0;
}

#undef ASSERT
