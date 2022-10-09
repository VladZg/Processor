#include "Stack/Config.h"
#include "Constants.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Stack/Stack.h"

// #define NDUMP

#include "DumpGraphics.h"
#include "TechInfo.h"
#include "CheckFile.h"

// #define PATRIOTIC_CPU
#ifdef PATRIOTIC_CPU
int RUSSIA = PrintRusFlag();
#endif

#undef FILENAME_INPUT
#undef FILENAME_INPUT_DEFAULT

const char  FILENAME_INPUT_DEFAULT[]  = "Source_output.asm";
const char* FILENAME_INPUT            = nullptr;

int REGS[5] = {0, 0, 0, 0, 0};

struct Cpu
{
    char*  code;
    int    code_size;
    size_t ip;

    int*   RAM;
    Stack  stack;

    int*   Regs;
};

int* GetArg     (Cpu* cpu, int cmd, int* arg);
int  GetRAM     (Cpu* cpu, int index);
void CpuCtor    (Cpu* cpu, int code_size, FILE* file);
void CpuCleaner (Cpu* cpu);
void FullDump   (Cpu* cpu);
void CpuError   (Cpu* cpu, FILE* file, int err_code);

int main(int argc, char** argv)
{
    if (!CheckFile(argc, argv, &FILENAME_INPUT))
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;

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
            int cmd = cpu.code[cpu.ip];

            switch(cmd & CMD_CODE_MASK)
            {
                case CMD_PUSH:
                {
                    int arg_temp = 0;
                    cpu.ip++;
                    int* arg = GetArg(&cpu, cmd, &arg_temp);
                    StackPush(&cpu.stack, *arg);

                    SimpleStackDump(&cpu.stack, "Push");

                    break;
                }

                case CMD_POP:
                {
                    int arg_temp = 0;
                    int* arg = 0;

                    cpu.ip++;

                    if (cmd & ARG_MEM)
                    {
                        cmd &= ~ARG_MEM;
                        arg = &cpu.RAM[*GetArg(&cpu, cmd, &arg_temp)];
                    }

                    else if ((cmd & ARG_REG) && !(cmd & ARG_IMMED))
                    {
                        arg = &cpu.Regs[cpu.code[cpu.ip]];
                        cpu.ip += sizeof(int);
                    }

                    else
                    {
                        fprintf(stderr, "Wrong type of argument\n");
                        exit(1);
                    }

                    *arg = StackPop(&cpu.stack);

                    SimpleStackDump(&cpu.stack, "Pop");

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

                    if (num)
                    {
                        StackPush(&cpu.stack, StackPop(&cpu.stack) / num);

                        SimpleStackDump(&cpu.stack, "Div");
                        cpu.ip++;
                    }

                    else
                        CpuError(&cpu, file, DIV_ON_ZERO_ERR_CODE);

                    break;
                }

                case CMD_OUT:
                {
                    int popped = StackPop(&cpu.stack);
                    SimpleStackDump(&cpu.stack, "Out");
                    fprintf(stderr, "    %d\n", popped);

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
                    FullDump(&cpu);
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
    cpu->Regs = REGS;

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

int GetRAM(Cpu* cpu, int index)
{
    sleep(GET_RAM_DELAY);

    return cpu->RAM[index];
}

int* GetArg(Cpu* cpu, int cmd, int* arg)
{
    // fprintf(stderr, "1: %d %d %d %d\n", cmd, cmd & ARG_IMMED, cmd & ARG_REG, cmd & ARG_MEM);

    if (cmd & ARG_IMMED)
    {
        *arg    += *(int*)(cpu->code + cpu->ip);
        cpu->ip += sizeof(int);
    }

    if (cmd & ARG_REG)
    {
        *arg    += cpu->Regs[*(int*)(cpu->code + cpu->ip)];
        cpu->ip += sizeof(int);
    }

    if (cmd & ARG_MEM)
        *arg = GetRAM(cpu, *arg);

    // fprintf(stderr, "\n2: %d\n", *arg);

    return arg;
}

#ifndef NDUMP

void FullDump(Cpu* cpu)
{
    fprintf(stderr, "  \\\\");

    WriteNSymb(5 * cpu->code_size + 5, '=');

    fprintf(stderr, "\\\\\n    ip:   ");

    for (int i = 0; i < cpu->code_size; i++)
    {
        if (i != cpu->ip)
            fprintf(stderr, "%04d ", i);
        else
            fprintf(stderr, KYEL "%04d " KNRM, i);
    }

    fprintf(stderr, "\n    code: ");

    for (int i = 0; i < cpu->code_size; i++)
    {
        if (i != cpu->ip)
            fprintf(stderr, "%04d ", cpu->code[i]);
        else
            fprintf(stderr, KYEL "%04d " KNRM, cpu->code[i]);
    }

    fprintf(stderr, "\n    ");

    WriteNSymb(5 * cpu->ip + 6, '-');

    fprintf(stderr, KYEL "^ ip = %ld\n" KNRM, cpu->ip);

    SimpleStackDump_(&cpu->stack);

    fprintf(stderr, "\n    registers:\n    {\n");

    for (int i = 1; i <= REGS_SIZE; i++)
        fprintf(stderr, "        *[%d] = %d\n",  i, cpu->Regs[i]);

    fprintf(stderr, "    }\n  \\\\");

    WriteNSymb(5 * cpu->code_size + 5, '=');

    fprintf(stderr, "\\\\\n\n");
}

#else

void FullDump(Cpu* cpu) {}

#endif

void CpuError(Cpu* cpu, FILE* file, int err_code)
{
    switch(err_code)
    {
        case DIV_ON_ZERO_ERR_CODE:
        {
            fprintf(stderr, "  ERROR: Division on zero");

            break;
        }
    }

    fprintf(stderr, " in line (? эта функция пока недоступна, ха-ха)\n");

    CpuCleaner(cpu);
    fclose(file);
    exit(1);
}
