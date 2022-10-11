#include "Stack/Config.h"
#include "Constants.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Stack/Stack.h"
#include "Description.h"

// #define NDUMP

#include "CpuGraphics.h"
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

struct Cpu
{
    char*  code;
    int    code_size;
    size_t ip;

    int*   RAM;
    Stack  stack;

    int*   Regs;
};

#define IP    cpu.ip
#define CODE  cpu.code
#define STACK cpu.stack

int  DoCpuCycle (const char* filename_input);

int* GetArg     (Cpu* cpu, int cmd, int* arg);
int* GetRAM     (Cpu* cpu, int index);

void CpuCtor    (Cpu* cpu, int code_size, FILE* file);
void CpuCleaner (Cpu* cpu);
void CpuError   (Cpu* cpu, FILE* file, int err_code);

void PrintCmdName (const char* cmd);
void RAMDump      (int* RAM);
void RegsDump     (int* Regs);
void FullDump     (Cpu* cpu, char ip_min, char ip_max);

int IsFridayToday();

int main(int argc, char** argv)
{
    PrintHelp(argc, argv);

    if (!CheckFile(argc, argv, &FILENAME_INPUT))
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;

    DoCpuCycle(FILENAME_INPUT);

    return 0;
}


int IsFridayToday()
{
    time_t now        = time(0);
    tm     *real_time = localtime(&now);

    return abs(real_time->tm_wday - 2);
}

void CpuCtor(Cpu* cpu, int code_size, FILE* file)
{
    cpu->Regs = START_REGS;

    cpu->code_size = code_size;

    cpu->code = (char*) calloc(code_size, sizeof(char));
    ASSERT(cpu->code != NULL);

    cpu->RAM = (int*) calloc(RAM_SIZE + 1, sizeof(int));
    ASSERT(cpu->RAM != NULL);

    for (int i = 0; i <= RAM_SIZE; i++) cpu->RAM[i] = RAM_POISON;

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

int* GetRAM(Cpu* cpu, int index)
{
    fprintf(stderr, "  Getting RAM[%d] value", index);

    PrintLoading(GET_RAM_DELAY);

    return &cpu->RAM[index];
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
        arg = GetRAM(cpu, *arg);

    // fprintf(stderr, "\n2: %d\n", *arg);

    return arg;
}

#ifndef NDUMP

void PrintCmdName(const char* cmd)
{
    fprintf(stderr, KRED "  %s:\n" KNRM, cmd);
}

void RAMDump(int* RAM)
{
    fprintf(stderr, "    RAM (not empty cells):\n    {\n");

    int count_empties = 0;

    for (int i = 0; i <= RAM_SIZE; i++)
    {
        int num = RAM[i];

        if (num != RAM_POISON)
        {
            fprintf(stderr, "        *[%d] = " KMAG "%d\n" KNRM,  i, num);
            count_empties++;
        }
    }

    if (!count_empties) fprintf(stderr, KBLU "        RAM is empty\n" KNRM);

    fprintf(stderr, "    }\n\n");
}

void RegsDump(int* Regs)
{
    fprintf(stderr, "    Registers:\n    {\n");

    for (int i = 1; i <= REGS_SIZE; i++)
        fprintf(stderr, "        *[%d] = " KMAG "%d\n" KNRM,  i, Regs[i]);

    fprintf(stderr, "    }\n\n");
}

void FullDump(Cpu* cpu, char ip_min, char ip_max)
{
    size_t ip_now = cpu->ip;

    if ((ip_min != -1) && (ip_max <= cpu->code_size - 1))
        fprintf(stderr, KRED "  Dump (ip: %d -> %d):\n" KNRM, ip_min, ip_max);

    else
    {
        fprintf(stderr, "  Dump (full code):\n");
        ip_min = 0;
        ip_max = cpu->code_size - 1;
    }

    fprintf(stderr, "  \\\\");

    WriteNSymb(176, '=');

    fprintf(stderr, "\\\\\n\n    ip:   ");

    for (int i = ip_min; i <= ip_max; i++)
    {
        if (i != ip_now)
            fprintf(stderr, "%04d ", i);
        else
            fprintf(stderr, KYEL "%04d " KNRM, i);
    }

    fprintf(stderr, "\n    code: ");

    for (int i = ip_min; i <= ip_max; i++)
    {
        if (i != ip_now)
            fprintf(stderr, "%04d ", cpu->code[i]);
        else
            fprintf(stderr, KYEL "%04d " KNRM, cpu->code[i]);
    }

    fprintf(stderr, "\n    ");

    if ((ip_now >= ip_min) && (ip_now <= ip_max))
    {
        WriteNSymb(5 * (ip_now - ip_min) + 6, '-');
        fprintf(stderr, KYEL "^ " KNRM);
    }

    fprintf(stderr, KYEL "ip = %ld\n\n" KNRM, ip_now);

    SimpleStackDump_(&cpu->stack);

    fprintf(stderr, "\n");

    RegsDump(cpu->Regs);

    RAMDump(cpu->RAM);

    fprintf(stderr, "  \\\\");

    WriteNSymb(176, '=');

    fprintf(stderr, "\\\\\n\n");
}

#else

void PrintCmdName(const char* cmd) {}
void RegsDump(int* Regs) {}
void RAMDump(int* RAM) {}
void FullDump(Cpu* cpu, char ip_min, char ip_max) {}

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

int DoCpuCycle(const char* filename_input)
{
    FILE* file = fopen(filename_input, "rb");
    ASSERT(file != NULL);

    TechInfo tech_info = {};
    fread(&tech_info, sizeof(int), TECH_INFO_SIZE, file);

    if ((tech_info.filecode == CP_FILECODE) && (tech_info.version == CMD_VERSION))
    {
        Cpu cpu = {};
        CpuCtor(&cpu, tech_info.code_size, file);

        while (IP < cpu.code_size)
        {
            fprintf(stderr, "  Processing");
            PrintLoading(BETW_STEPS_DELAY);

            int cmd = CODE[IP];

            switch(cmd & CMD_CODE_MASK)
            {
                case CMD_PUSH:
                {
                    PrintCmdName("Push");

                    int arg_temp = 0;
                    IP++;
                    int* arg = GetArg(&cpu, cmd, &arg_temp);

                    if (*arg != RAM_POISON)
                        StackPush(&STACK, *arg);

                    else
                    {
                        fprintf(stderr, "    ERROR: ARGUMENT OF PUSH IS POISONED ELEM FROM THE RAM\n");

                        CpuCleaner(&cpu);
                        fclose(file);

                        exit(1);
                    }

                    SimpleStackDump(&STACK);

                    break;
                }

                case CMD_POP:
                {
                    PrintCmdName("Pop");

                    int arg_temp = 0;
                    int* arg = 0;

                    IP++;

                    if (cmd & ARG_MEM)
                    {
                        cmd &= ~ARG_MEM;
                        arg = GetArg(&cpu, cmd, &arg_temp);

                        if ((*arg <= RAM_SIZE) && (*arg >= 0))
                            arg = GetRAM(&cpu, *arg);

                        else
                        {
                            fprintf(stderr, "  ERROR: ARGUMENT OF POP (%d) IS OUT OF RAM RANGE\n", *arg);
                            CpuCleaner(&cpu);
                            exit(1);
                        }
                    }

                    else if ((cmd & ARG_REG) && !(cmd & ARG_IMMED))
                    {
                        arg = &cpu.Regs[CODE[IP]];
                        IP += sizeof(int);
                    }

                    else
                    {
                        fprintf(stderr, "Wrong type of argument\n");
                        exit(1);
                    }

                    *arg = StackPop(&STACK);

                    SimpleStackDump(&STACK);
                    RegsDump(cpu.Regs);
                    RAMDump(cpu.RAM);

                    break;
                }

                case CMD_ADD:
                {
                    PrintCmdName("Add");

                    StackPush(&STACK, StackPop(&STACK) + StackPop(&STACK));

                    SimpleStackDump(&STACK);

                    IP++;

                    break;
                }

                case CMD_SUB:
                {
                    PrintCmdName("Sub");

                    int num = StackPop(&STACK);

                    StackPush(&STACK, StackPop(&STACK) - num);

                    SimpleStackDump(&STACK);

                    IP++;

                    break;
                }

                case CMD_MUL:
                {
                    PrintCmdName("Mul");

                    StackPush(&STACK, StackPop(&STACK) * StackPop(&STACK));

                    SimpleStackDump(&STACK);

                    IP++;

                    break;
                }

                case CMD_DIV:
                {
                    PrintCmdName("Div");

                    int num = StackPop(&STACK);

                    if (num)
                    {
                        StackPush(&STACK, StackPop(&STACK) / num);

                        SimpleStackDump(&STACK);

                        IP++;
                    }

                    else
                        CpuError(&cpu, file, DIV_ON_ZERO_ERR_CODE);

                    break;
                }

                case CMD_OUT:
                {
                    PrintCmdName("Out");

                    int popped = StackPop(&STACK);

                    SimpleStackDump(&STACK);

                    fprintf(stderr, "    %d ", popped);

                    if (popped == RAM_POISON) fprintf(stderr, KGRN "(RAM poison)" KNRM);

                    fprintf(stderr, "\n\n");

                    IP++;

                    break;
                }

                case CMD_PIN:
                {
                     PrintCmdName("Pin");

                    int num = 0;

                    fprintf(stderr, "  Type a number, it'll be used in calculatings: ");
                    scanf("%d", &num);

                    StackPush(&STACK, num);

                    SimpleStackDump(&STACK);

                    IP++;

                    break;
                }

                case CMD_JMP:
                {
                    IP++;

                    fprintf(stderr, "  jumping from %ld to %d", IP, CODE[IP]);
                    PrintLoading(JUMP_DELAY);

                    IP = CODE[IP];

                    break;
                }

                case CMD_JB:
                {
                    IP++;

                    if (StackPop(&STACK) > StackPop(&STACK))
                    {
                        fprintf(stderr, "  jumping from %ld to %d", IP, CODE[IP]);
                        PrintLoading(JUMP_DELAY);

                        IP = CODE[IP];
                    }

                    else IP += sizeof(int);

                    break;
                }

                case CMD_JBE:
                {
                    IP++;

                    if (StackPop(&STACK) >= StackPop(&STACK))
                    {
                        fprintf(stderr, "  jumping from %ld to %d", IP, CODE[IP]);
                        PrintLoading(JUMP_DELAY);

                        IP = CODE[IP];
                    }

                    else IP += sizeof(int);

                    break;
                }

                case CMD_JA:
                {
                    IP++;

                    if (StackPop(&STACK) < StackPop(&STACK))
                    {
                        fprintf(stderr, "  jumping from %ld to %d", IP, CODE[IP]);
                        PrintLoading(JUMP_DELAY);

                        IP = CODE[IP];
                    }

                    else IP += sizeof(int);

                    break;
                }

                case CMD_JAE:
                {
                    IP++;

                   if (StackPop(&STACK) <= StackPop(&STACK))
                    {
                        fprintf(stderr, "  jumping from %ld to %d", IP, CODE[IP]);
                        PrintLoading(JUMP_DELAY);

                        IP = CODE[IP];
                    }

                    else IP += sizeof(int);

                    break;
                }

                case CMD_JE:
                {
                    IP++;

                    if (StackPop(&STACK) == StackPop(&STACK))
                    {
                        fprintf(stderr, "  jumping from %ld to %d", IP, CODE[IP]);
                        PrintLoading(JUMP_DELAY);

                        IP = CODE[IP];
                    }

                    else IP += sizeof(int);

                    break;
                }

                case CMD_JNE:
                {
                    IP++;

                    if (StackPop(&STACK) != StackPop(&STACK))
                    {
                        fprintf(stderr, "  jumping from %ld to %d", IP, CODE[IP]);
                        PrintLoading(JUMP_DELAY);

                        IP = CODE[IP];
                    }

                    else IP += sizeof(int);

                    break;
                }

                case CMD_JF:
                {
                    IP++;

                    if (IsFridayToday())
                    {
                        fprintf(stderr, "  jumping from %ld to %d", IP, CODE[IP]);
                        PrintLoading(JUMP_DELAY);

                        IP = CODE[IP];
                    }

                    else IP += sizeof(int);

                    break;
                }

                case CMD_HLT:
                {
                    PrintCmdName("Hlt");

                    fprintf(stderr, "    Program \"%s\" has finished correctly\n", FILENAME_INPUT);

                    CpuCleaner(&cpu);
                    fclose(file);

                    return 1;
                }

                case CMD_DUMP:
                {
                    char ip_min = CODE[IP + 1];
                    char ip_max = CODE[IP + 2];

                    FullDump(&cpu, ip_min, ip_max);

                    IP += 3;

                    break;
                }

                default:
                {
                    fprintf(stderr, "  NO SUCH COMMAND WITH CODE %d\n  FILE \"%s\" IS DAMAGED!!!\n", CODE[IP], FILENAME_INPUT);

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
