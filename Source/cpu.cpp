#include "../Libs/Stack/Include/Config.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "../Libs/Stack/Include/Stack.h"
#include "../Libs/Onegin/Include/functions.h"
#include "../Include/Constants.h"
#include "../Include/Description.h"

#define NDUMP
// #define DELAYS_MODE

#include "../Include/CpuGraphics.h"
#include "../Include/TechInfo.h"
#include "../Include/CheckFile.h"

#ifdef NDEBUG
#undef ASSERT
#define ASSERT(condition) {}
#endif

// #define PATRIOTIC_CPU
#ifdef PATRIOTIC_CPU
int RUSSIA = PrintRusFlag();
#endif

#undef FILENAME_INPUT
#undef FILENAME_INPUT_DEFAULT

const char  FILENAME_INPUT_DEFAULT[]  = "./Source.exe";
const char* FILENAME_INPUT            = nullptr;

struct Cpu
{
    char*  code;
    int    code_size;
    size_t ip;

    int*   RAM;
    Stack  stack;

    Stack stack_addr_ret;

    int*   Regs;
};

#define IP    cpu.ip
#define CODE  cpu.code
#define STACK cpu.stack

void CpuCtor    (Cpu* cpu, int code_size, FILE* file);
void CpuCleaner (Cpu* cpu);
int  CpuError   (Cpu* cpu, FILE* file, int err_code, const char* fmt_err_msg, ...);
int  DoCpuCycle (const char* filename_input);

int* GetArg     (Cpu* cpu, int cmd, int* arg);
int* GetRAM     (Cpu* cpu, int index);

int IsFridayToday();

void PrintCmdName (const char* cmd);
void RAMDump      (int* RAM);
void RegsDump     (int* Regs);
void FullDump     (Cpu* cpu, char ip_min, char ip_max);

int main(const int argc, const char** argv)
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

    return !abs(real_time->tm_wday - 5);
}

void CpuCtor(Cpu* cpu, int code_size, FILE* file)
{
    ASSERT(cpu != NULL);
    ASSERT(file != NULL);

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

    cpu->stack_addr_ret = {};
    StackCtor(cpu->stack_addr_ret);
}

void CpuCleaner(Cpu* cpu)
{
    ASSERT(cpu != NULL);

    StackDtor(&cpu->stack);
    StackDtor(&cpu->stack_addr_ret);
    free(cpu->RAM);
    free(cpu->code);
}

int* GetRAM(Cpu* cpu, int index)
{
    ASSERT(cpu != NULL);

    PrintLoading(GET_RAM_DELAY, "  Getting RAM[%d] value", index);

    return &cpu->RAM[index];
}

int* GetArg(Cpu* cpu, int cmd, int* arg)
{
    ASSERT(cpu != NULL);
    ASSERT(arg != NULL)

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
    {
        if ((*arg <= RAM_SIZE) && (*arg >= 0))
            arg = GetRAM(cpu, *arg);

        else CpuError(cpu, stderr, ARG_ERR_CODE, "ARGUMENT OF PUSH IS WRONG, "
                                                 "ITS INDEX (%d) OUT OF RAM RANGE", *arg);
    }

    return arg;
}

#ifndef NDUMP

void PrintCmdName(const char* cmd)
{
    ASSERT(cmd != NULL);

    fprintf(stderr, KRED "  %s\n" KNRM, cmd);
}

void RAMDump(int* RAM)
{
    ASSERT(RAM != NULL);

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
    ASSERT(Regs != NULL);

    fprintf(stderr, "    Registers:\n    {\n");

    for (int i = 1; i <= REGS_SIZE; i++)
        fprintf(stderr, "        *[%d] = " KMAG "%d\n" KNRM,  i, Regs[i]);

    fprintf(stderr, "    }\n\n");
}

void FullDump(Cpu* cpu, char ip_min, char ip_max)
{
    ASSERT(cpu != NULL);

    size_t ip_now = cpu->ip;

    if ((ip_min != -1) && (ip_max <= cpu->code_size - 1))
        fprintf(stderr, "  \\\\ip: %d -> %d)\n", ip_min, ip_max);

    else
    {
        fprintf(stderr, "  \\\\full code\n");
        ip_min = 0;
        ip_max = cpu->code_size - 1;
    }

    fprintf(stderr, "  \\\\");

    WriteNSymb(176, '=');

    fprintf(stderr, "\\\\\n\n" KBLU "    ip:   " KNRM);

    for (int i = ip_min; i <= ip_max; i++)
    {
        if (i != ip_now)
            fprintf(stderr, "%04d ", i);
        else
            fprintf(stderr, KYEL "%04d " KNRM, i);
    }

    fprintf(stderr, KBLU "\n    code: " KNRM);

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

    fprintf(stderr, "    Stack of return addresses:\n    {\n");

    SimpleStackDump_(&cpu->stack_addr_ret);

    fprintf(stderr, "    }\n");

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

int CpuError(Cpu* cpu, FILE* file, int err_code, const char* fmt_err_msg, ...)
{
    ASSERT(cpu != NULL);
    ASSERT(file != NULL);
    ASSERT(fmt_err_msg != NULL);

    fprintf(stderr, KYEL "    ERROR (%d): \n    ", err_code);

    va_list args;
    va_start(args, fmt_err_msg);

    vfprintf(stderr, fmt_err_msg, args);
    fprintf(stderr, KNRM KMAG "\n\n  PROGRAM STOPPED!!!\n" KNRM);

    va_end(args);

    CpuCleaner(cpu);
    fclose(file);
    return 0;
    // exit(1);
}

#define DEF_CMD(name, num, arg, ...)                                   \
    case CMD_##name:                                                   \
    {                                                                  \
        PrintCmdName(#name);                                           \
                                                                       \
        IP++;                                                          \
        __VA_ARGS__                                                    \
                                                                       \
        break;                                                         \
    }

#define DEF_JMP(name, num, condition, ...)                             \
    DEF_CMD(name, num, 1,                                              \
    {                                                                  \
        if (condition)                                                 \
        {                                                              \
            __VA_ARGS__                                                \
                                                                       \
            PrintLoading(JUMP_DELAY, "  Jumping from %ld to %d",       \
                            IP, CODE[IP]);                             \
                                                                       \
            IP = *(int*)(CODE + IP);                                   \
        }                                                              \
                                                                       \
        else IP += sizeof(int);                                        \
    })

#define DEF_DUMP(name, num)                                            \
    DEF_CMD(name, num, 1,                                              \
    {                                                                  \
        char ip_min = CODE[IP++];                                      \
        char ip_max = CODE[IP++];                                      \
                                                                       \
        FullDump(&cpu, ip_min, ip_max);                                \
    })

int DoCpuCycle(const char* filename_input)
{
    ASSERT(filename_input != NULL);

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
            PrintLoading(BETW_STEPS_DELAY, "  Processing");

            int cmd = CODE[IP];

            switch(cmd & CMD_CODE_MASK)
            {
                #include "../Include/Cmd.h"

                default:
                    CpuError(&cpu, file, SYNTAX_ERR_CODE, "  NO SUCH COMMAND WITH CODE %d\n"
                                                          "  FILE \"%s\" IS DAMAGED!!!", CODE[IP], FILENAME_INPUT);
            }
        }

        CpuCleaner(&cpu);
        fclose(file);

        return 1;
    }

    else if (tech_info.filecode != CP_FILECODE)
        fprintf(stderr, KYEL "  WRONG TYPE OF ASM FILE!!!\n"
                             "  YOU HAVE TO USE CP_FILECODE \"%d\"\n" KNRM, CP_FILECODE);

    else if (tech_info.version != CMD_VERSION)
        fprintf(stderr, KYEL "  USED OLD (\"%d\") VERSION OF COMMANDS!!!\n"
                             "  YOU HAVE TO USE THE \"%d\" VERSION!!!\n" KNRM, tech_info.version, CMD_VERSION);

    fclose(file);
    return 0;
    // exit(1);
}

#undef DEF_DUMP
#undef DEF_JMP
#undef DEF_CMD
