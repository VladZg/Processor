#include "Stack/Config.h"
#include "Constants.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cstdint>
#include "Stack/Stack.h"
// #define NDUMP

#include "DumpGraphics.h"
#include "TechInfo.h"
#include "CheckFile.h"

#undef FILENAME_INPUT
#undef FILENAME_INPUT_DEFAULT

const char  FILENAME_INPUT_DEFAULT[]  = "Source_output.asm";
const char* FILENAME_INPUT            = nullptr;

struct Cpu
{
    char*  code;
    int    code_size;
    size_t ip;

    int*   RAM;
    Stack  stack;

    int*   regs;
};

int* GetArg(Cpu* cpu);
int  GetRAM(int* RAM);
void CpuCtor(Cpu* cpu, int code_size, FILE* file);
void CpuCleaner(Cpu* cpu);


int main(int argc, char** argv)
{
    if (argc == 2)
    {
        if (CheckFile(argv[1]))
            FILENAME_INPUT = argv[1];

        else
        {
            fprintf(stderr,
                    "\n  \"cpu\":\n"
                    "  NO FILE \"%s\" IN THIS DIRECTORY\n\n", argv[1]);
            exit(1);
        }
    }

    else
    {
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;
    }

    FILE* file = fopen(FILENAME_INPUT, "rb");
    ASSERT(file != NULL);

    TechInfo tech_info = {};
    fread(&tech_info, sizeof(int), TECH_INFO_SIZE, file);

    if ((tech_info.filecode == CP_FILECODE) && (tech_info.version == CMD_VERSION))
    {
        Cpu cpu = {};
        CpuCtor(&cpu, tech_info.code_size, file);

        while (cpu.ip < cpu.code_size)
        {
            switch(cpu.code[cpu.ip])
            {
                case CMD_PUSH:
                {
                    cpu.ip++;
                    StackPush(&cpu.stack, *(int*)(cpu.code + cpu.ip));
                    SimpleStackDump(&cpu.stack, "Push");
                    cpu.ip += sizeof(int);

                    break;
                }

                case CMD_ADD:
                {
                    StackPush(&cpu.stack, StackPop(&cpu.stack) + StackPop(&cpu.stack));
                    SimpleStackDump(&cpu.stack, "Add");
                    cpu.ip++;

                    break;
                }

                case CMD_SUB:
                {
                    int num = StackPop(&cpu.stack);

                    StackPush(&cpu.stack, StackPop(&cpu.stack) - num);
                    SimpleStackDump(&cpu.stack, "Sub");
                    cpu.ip++;

                    break;
                }

                case CMD_MUL:
                {
                    StackPush(&cpu.stack, StackPop(&cpu.stack) * StackPop(&cpu.stack));
                    SimpleStackDump(&cpu.stack, "Mul");
                    cpu.ip++;

                    break;
                }

                case CMD_DIV:
                {
                    int num = StackPop(&cpu.stack);

                    StackPush(&cpu.stack, StackPop(&cpu.stack) / num);
                    SimpleStackDump(&cpu.stack, "Div");
                    cpu.ip++;

                    break;
                }

                case CMD_OUT:
                {
                    int popped = StackPop(&cpu.stack);
                    SimpleStackDump(&cpu.stack, "Out");
                    fprintf(stderr, "  Output: %d\n\n", popped);
                    cpu.ip++;

                    break;
                }

                case CMD_PIN:
                {
                    int num = 0;

                    fprintf(stderr, "  Type a number, it'll be used in calculatings: ");
                    scanf("%d", &num);

                    StackPush(&cpu.stack, num);
                    SimpleStackDump(&cpu.stack, "Pin");
                    cpu.ip++;

                    break;
                }

                case CMD_HLT:
                {
                    fprintf(stderr, "  Program \"%s\" has finished correctly\n", FILENAME_INPUT);

                    CpuCleaner(&cpu);
                    fclose(file);

                    exit(1);

                    break;
                }

                case CMD_DUMP:
                {
                    FullDump(cpu.code, cpu.code_size, cpu.ip, &cpu.stack);
                    cpu.ip++;

                    break;
                }

                default:
                {
                    fprintf(stderr, "  NO SUCH COMMAND WITH CODE %d\n  FILE \"%s\" IS DAMAGED!!!\n", cpu.code[cpu.ip], FILENAME_INPUT);

                    CpuCleaner(&cpu);
                    fclose(file);

                    exit(1);
                }
            }
        }

        CpuCleaner(&cpu);
        fclose(file);
    }

    else if (tech_info.filecode != CP_FILECODE)
    {
        fprintf(stderr, "  WRONG TYPE OF ASM FILE!!!\n  YOU HAVE TO USE CP_FILECODE \"%d\"\n", CP_FILECODE);

        fclose(file);

        exit(1);
    }

    else if (tech_info.version != CMD_VERSION)
    {
        fprintf(stderr, "  USED OLD (\"%d\") VERSION OF COMMANDS!!!\n  YOU HAVE TO USE THE \"%d\" VERSION!!!\n",
                tech_info.version, CMD_VERSION);

        fclose(file);

        exit(1);
    }

    return 0;
}

void CpuCtor(Cpu* cpu, int code_size, FILE* file)
{
    cpu->code_size = code_size;

    cpu->code = (char*) calloc(code_size, sizeof(char));
    ASSERT(cpu->code != NULL);

    cpu->RAM = (int*) calloc(RAM_SIZE, sizeof(int));
    ASSERT(cpu->RAM != NULL);

    fread(cpu->code, sizeof(char), code_size, file);

    cpu->ip = 0;

    cpu->stack = {};
    StackCtor(cpu->stack);
}

void CpuCleaner(Cpu* cpu)
{
    StackDtor(&cpu->stack);
    free(cpu->RAM);
    free(cpu->code);
}

int* GetArg(Cpu* cpu)
{
    int  cmd = cpu->code[cpu->ip++];
    int* arg = NULL;

    if (cmd & ARG_IMMED)
    {
        arg     += *(int*)(cpu->code + cpu->ip)
        cpu->ip += sizeof(int);
    }

    if (cmd & ARG_REG)
        arg += REGS[cpu->code[cpu->ip++]];

    // if (cmd & ARG_MEM)
    //     arg = cpu->RAM[*arg];

    return arg;
}

int  GetRAM(int* RAM, int index)
{
    sleep(GET_RAM_DELAY);

    return RAM[index];
}
