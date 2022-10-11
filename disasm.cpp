#include "Constants.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "TechInfo.h"
#include "CheckFile.h"
#include "Description.h"

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

void PrintArgs(char cmd, char* code, int* ip, FILE* file_out);
const char* ConvertReg(int reg_code);
int  Decompile(const char* filename_input, const char* filename_output);


int main(int argc, char** argv)
{
    PrintHelp(argc, argv);

    if (!CheckFile(argc, argv, &FILENAME_INPUT))
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;

    Decompile(FILENAME_INPUT, FILENAME_OUTPUT);

    return 0;
}


int Decompile(const char* filename_input, const char* filename_output)
{
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
            char cmd = code[ip];

            switch(cmd & CMD_CODE_MASK)
            {
                case CMD_PUSH:
                {
                    ip++;
                    fprintf(file_out, "Push ");
                    PrintArgs(cmd, code, &ip, file_out);
                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_POP:
                {
                    ip++;
                    fprintf(file_out, "Pop ");
                    PrintArgs(cmd, code, &ip, file_out);
                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_ADD:
                {
                    ip++;
                    fprintf(file_out, "Add\n");

                    break;
                }

                case CMD_SUB:
                {
                    ip++;
                    fprintf(file_out, "Sub\n");

                    break;
                }

                case CMD_MUL:
                {
                    ip++;
                    fprintf(file_out, "Mul\n");

                    break;
                }

                case CMD_DIV:
                {
                    ip++;
                    fprintf(file_out, "Div\n");

                    break;
                }

                case CMD_OUT:
                {
                    ip++;
                    fprintf(file_out, "Out\n");

                    break;
                }

                case CMD_PIN:
                {
                    ip++;
                    fprintf(file_out, "Pin\n");

                    break;
                }

                case CMD_HLT:
                {
                    ip++;
                    fprintf(file_out, "Hlt\n");

                    break;
                }

                case CMD_DUMP:
                {
                    ip++;
                    fprintf(file_out, "Dump ");
                    fprintf(file_out, "%d ", *(code + (ip++)));
                    fprintf(file_out, "%d\n", *(code + (ip++)));

                    break;
                }

                case CMD_JMP:
                {
                    ip++;
                    fprintf(file_out, "Jmp ");
                    fprintf(file_out, "%d ", *(int*)(code + ip));

                    ip += sizeof(int);

                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_JB:
                {
                    ip++;

                    fprintf(file_out, "Jb ");
                    fprintf(file_out, "%d ", *(int*)(code + ip));

                    ip += sizeof(int);

                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_JBE:
                {
                    ip++;

                    fprintf(file_out, "Jbe ");
                    fprintf(file_out, "%d ", *(int*)(code + ip));

                    ip += sizeof(int);

                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_JA:
                {
                    ip++;

                    fprintf(file_out, "Ja ");
                    fprintf(file_out, "%d ", *(int*)(code + ip));

                    ip += sizeof(int);

                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_JAE:
                {
                    ip++;

                    fprintf(file_out, "Jae ");
                    fprintf(file_out, "%d ", *(int*)(code + ip));

                    ip += sizeof(int);

                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_JE:
                {
                    ip++;

                    fprintf(file_out, "Je ");
                    fprintf(file_out, "%d ", *(int*)(code + ip));

                    ip += sizeof(int);

                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_JNE:
                {
                    ip++;

                    fprintf(file_out, "Jne ");
                    fprintf(file_out, "%d ", *(int*)(code + ip));

                    ip += sizeof(int);

                    fprintf(file_out, "\n");

                    break;
                }

                case CMD_JF:
                {
                    ip++;

                    fprintf(file_out, "Jf ");
                    fprintf(file_out, "%d ", *(int*)(code + ip));
                    fprintf(file_out, "\n");

                    ip += sizeof(int);

                    break;
                }

                default:
                {
                    fprintf(stderr, "  NO SUCH COMMAND WITH CODE %d\n  FILE \"%s\" IS DAMAGED!!!\n", code[ip], FILENAME_INPUT);

                    fclose(file_inp);
                    fclose(file_out);

                    exit(1);
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

    return 1;
}

const char* ConvertReg(int reg_code)
{
    switch (reg_code)
    {
        case RAX_CODE:
            return "rax";

        case RBX_CODE:
            return "rbx";

        case RCX_CODE:
            return "rcx";

        case RDX_CODE:
            return "rdx";

        default:
        {
            fprintf(stderr, "ERROR: Wrong register code\n");
            exit(1);
        }
    }
}

void PrintArgs(char cmd, char* code, int* ip, FILE* file_out)
{
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
