#include "../Libs/Stack/Include/Config.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "../Include/Constants.h"
#include "../Include/TechInfo.h"
#include "../Include/CheckFile.h"
#include "../Include/Description.h"

#define  ASSERT( condition )                                                \
{                                                                           \
    if (!(condition))                                                       \
        fprintf(stderr, "Error in '%s' in line %d in file \"%s\" in %s\n",  \
                #condition, __LINE__, __FILE__, __PRETTY_FUNCTION__);       \
}

#undef FILENAME_INPUT
#undef FILENAME_OUTPUT
#undef FILENAME_INPUT_DEFAULT

const char  FILENAME_INPUT_DEFAULT[]  = "./Source.exe";
const char* FILENAME_INPUT            = nullptr;
const char  FILENAME_OUTPUT[]         = "./Source.dsm";

void PrintArgs(char cmd, char* code, int* ip, FILE* file_out);
const char* ConvertReg(int reg_code);
int  Decompile(const char* filename_input, const char* filename_output);


int main(const int argc, const char** argv)
{
    PrintHelp(argc, argv);

    if (!CheckFile(argc, argv, &FILENAME_INPUT))
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;

    Decompile(FILENAME_INPUT, FILENAME_OUTPUT);

    return 0;
}


int Decompile(const char* filename_input, const char* filename_output)
{
    ASSERT(filename_input != NULL)
    ASSERT(filename_output != NULL)

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

        #define DEF_CMD(name, num, arg, ...)                      \
            case CMD_##name:                                      \
            {                                                     \
                ip++;                                             \
                                                                  \
                fprintf(file_out, "%s ", #name);                  \
                                                                  \
                if (arg) PrintArgs(cmd, code, &ip, file_out);     \
                                                                  \
                fprintf(file_out, "\n");                          \
                                                                  \
                break;                                            \
            }

        #define DEF_JMP(name, num, ...)                           \
            case CMD_##name:                                      \
            {                                                     \
                ip++;                                             \
                                                                  \
                fprintf(file_out, "%s ", #name);                  \
                fprintf(file_out, "%d ", *(int*)(code + ip));     \
                                                                  \
                ip += sizeof(int);                                \
                                                                  \
                fprintf(file_out, "\n");                          \
                                                                  \
                break;                                            \
            }

        #define DEF_DUMP(name, num, ...)                          \
            case CMD_##name:                                      \
            {                                                     \
                ip++;                                             \
                                                                  \
                fprintf(file_out, "%s ", #name);                  \
                fprintf(file_out, "%d ", *(code + (ip++)));       \
                fprintf(file_out, "%d\n", *(code + (ip++)));      \
                                                                  \
                break;                                            \
            }

        while (ip < code_size)
        {
            char cmd = code[ip];

            switch(cmd & CMD_CODE_MASK)
            {
                #include "../Include/Cmd.h"

                default:
                {
                    fprintf(stderr, "  NO SUCH COMMAND WITH CODE %d\n"
                                    "  FILE \"%s\" IS DAMAGED!!!\n", code[ip], FILENAME_INPUT);

                    fclose(file_inp);
                    fclose(file_out);

                    // exit(1);
                    return 0;
                }
            }
        }

        #undef DEF_DUMP
        #undef DEF_JMP
        #undef DEF_CMD

        fclose(file_inp);
        fclose(file_out);
    }

    else if (tech_info.filecode != CP_FILECODE)
    {
        fprintf(stderr, "    WRONG TYPE OF ASM FILE!!!\n"
                        "    YOU HAVE TO USE CP_FILECODE \"%d\"\n\n", CP_FILECODE);
        fclose(file_inp);
        // exit(1);
        return 0;
    }

    else if (tech_info.version != CMD_VERSION)
    {
        fprintf(stderr, "    USED OLD (\"%d\") VERSION OF COMMANDS!!!\n"
                        "    YOU HAVE TO USE THE \"%d\" VERSION!!!\n\n",
                tech_info.version, CMD_VERSION);
        fclose(file_inp);
        // exit(1);
        return 0;
    }

    return 1;
}

#define DEF_REG( name, num )    \
    case REG_##name:            \
        return #name;

const char* ConvertReg(int reg_code)
{
    switch (reg_code)
    {
        #include "../Include/Reg.h"

        default:
        {
            fprintf(stderr, "ERROR: Wrong register code\n");
            // exit(1);
            return NULL;
        }
    }
}

#undef DEF_REG

void PrintArgs(char cmd, char* code, int* ip, FILE* file_out)
{
    ASSERT(code != NULL)
    ASSERT(ip != NULL)
    ASSERT(file_out != NULL)

    if (cmd & ARG_MEM)
        fprintf(file_out, "[");

    if (cmd & ARG_IMMED)
    {
        fprintf(file_out, "%d ", *(code + (*ip)));
        *ip += sizeof(int);
    }

    if ((cmd & ARG_IMMED) && (cmd & ARG_REG))
        fprintf(file_out, "+ ");

    if (cmd & ARG_REG)
    {
        fprintf(file_out, "%s ", ConvertReg(*(int*)(code + (*ip))));
        *ip += sizeof(int);
    }

    if (cmd & ARG_MEM)
    {
        fseek(file_out, -1, SEEK_CUR);
        fprintf(file_out, "]");
    }
}

#undef ASSERT
