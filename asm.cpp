#include "Constants.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "Onegin/defines.h"
#include "Onegin/functions.h"
#include "TechInfo.h"
#include "CheckFile.h"

#undef  FILENAME_INPUT
#undef  FILENAME_OUTPUT
#undef  FILENAME_INPUT_DEFAULT

const char  FILENAME_INPUT_DEFAULT[]  = "Source_default.txt";
const char* FILENAME_INPUT            = nullptr;
const char  FILENAME_OUTPUT[]         = "Source_output.asm";

// struct Cmd {
//             unsigned short unused : 1;
//             unsigned short mem    : 1;
//             unsigned short reg    : 1;
//             unsigned short immed  : 1;
//             unsigned short code   : 4;
//             };

void GetArg(char* cmdline, int* num_arg, int* reg_arg);
void CompilationError(size_t err_code);

int main(int argc, char** argv)
{
    if (!CheckFile(argc, argv, &FILENAME_INPUT))
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;

    char** text = nullptr;
    char*  data = nullptr;
    int    text_lines_amount = 0;

    FILE* file_out = fopen(FILENAME_OUTPUT, "wb");
    ASSERT(file_out != NULL);

    read_file_to_text(FILENAME_INPUT, &data, &text, &text_lines_amount);

    char* code = (char*) calloc(9 * text_lines_amount, sizeof(char));
    ASSERT(code != NULL);

    int line = 0, ip = 0;

    TechInfo tech_info = {
                          CP_FILECODE,
                          CMD_VERSION,
                          0
                         };

    while (line < text_lines_amount)
    {
        char* cmd = (char*) calloc(strlen(text[line]), sizeof(char));
        ASSERT(cmd != NULL);

        sscanf(text[line], " %s", cmd);
        char cmd_code = 0;
        // Cmd cmd_code = {};

        if (strcasecmp(cmd, "push") == 0)
        {
            int num_arg = 0;
            int reg_arg = 0;

            GetArg(text[line], &num_arg,  &reg_arg);

            int is_immed = num_arg || !(num_arg && reg_arg);
            int is_arg   = reg_arg;

            if (is_immed)
                cmd_code |= ARG_IMMED;

            if (is_arg)
                cmd_code |= ARG_REG;

            cmd_code |= CMD_PUSH;

            // fprintf(stderr, "\n%d + %d -> %d\n\n", num_arg, reg_arg, cmd_code);

            code[ip++] = cmd_code;

            if (is_immed)
            {
                *(int*)(code + ip) = num_arg;
                ip += sizeof(int);
                tech_info.code_size += sizeof(int);
            }

            if (is_arg)
            {
                *(int*)(code + ip) = reg_arg;
                ip += sizeof(int);
                tech_info.code_size += sizeof(int);
            }
        }

        else if (strcasecmp(cmd, "pop") == 0)
        {
            code[ip++] = cmd_code | CMD_POP;
        }

        else if (strcasecmp(cmd, "add") == 0)
            code[ip++] = cmd_code | CMD_ADD;

        else if (strcasecmp(cmd, "sub") == 0)
            code[ip++] = cmd_code | CMD_SUB;

        else if (strcasecmp(cmd, "mul") == 0)
            code[ip++] = cmd_code | CMD_MUL;

        else if (strcasecmp(cmd, "div") == 0)
            code[ip++] = cmd_code | CMD_DIV;

        else if (strcasecmp(cmd, "out") == 0)
            code[ip++] = cmd_code | CMD_OUT;

        else if (strcasecmp(cmd, "pin") == 0)
            code[ip++] = cmd_code | CMD_PIN;

        else if (strcasecmp(cmd, "hlt") == 0)
            code[ip++] = cmd_code | CMD_HLT;

        else if (strcasecmp(cmd, "dump") == 0)
            code[ip++] = cmd_code | CMD_DUMP;

        else
        {
            fprintf(stderr, "    THERE IS NO COMMAND CALLED \"%s\" IN \"%d\" VERSION!!!\n"
                            "    COMPILED FILE WILL BE DAMAGED!!!\n\n", cmd, CMD_VERSION);

            tech_info.version = WRNG_CMD_VERSION;

            fwrite((const void*) &tech_info, sizeof(int), TECH_INFO_SIZE, file_out);

            exit(1);
        }

        tech_info.code_size++;
        free(cmd);
        line++;
    }

    FILE* file1 = fopen("test.txt", "wb");

    fwrite((const void*) &tech_info, sizeof(int), TECH_INFO_SIZE, file_out);
    fwrite((const void*) code, sizeof(char), tech_info.code_size, file_out);

    fclose(file_out);
    free(text);
    free(data);
    free(code);

    return 0;
}

void CompilationError(size_t err_code)
{
    switch (err_code)
    {
        case SYNTAX_ERR_CODE:
        {
            fprintf(stderr,"  SyntaxError\n");
            break;
        }
    }

    exit(1);
}

int IsRegister(char* str)
{
    if (strcasecmp(str, "rax") == 0)
        return RAX_CODE;

    else if (strcasecmp(str, "rbx") == 0)
        return RBX_CODE;

    else if (strcasecmp(str, "rcx") == 0)
        return RCX_CODE;

    else if (strcasecmp(str, "rdx") == 0)
        return RDX_CODE;

    else
        CompilationError(SYNTAX_ERR_CODE);

    return 0;
}

void GetArg(char* cmdline, int* num_arg, int* reg_arg)
{
    int arg1_shift = 0;   // arg1_shift - сдвиг первого аргумента отн начала строки
    int arg2_shift = 0;   // arg2_shift - 0, если нет второго аргумента, если есть - то это сдвиг от начала
    // char* arg2 = NULL;
    // int num1 = 0, num2 = 0;
    char* reg = NULL;

    // int k = sscanf(cmdline, "%*s %n%d + %n%*s", &arg1_shift, &num1, &arg2_shift);   // k отвечает за то, есть ли 1й числовой аргумент
    int k = sscanf(cmdline, "%*s %n%d + %n%*s", &arg1_shift, num_arg, &arg2_shift);

    if (k > 0) //если есть num1
    {
        if (arg2_shift) // если есть второй аргумент
        {
            // arg2 = cmdline + arg2_shift;
            reg = cmdline + arg2_shift;

            // if (sscanf(arg2, "%d", &num2) == 1);  //если второй аргумент - число

            // else
            //   *reg_code = IsRegister(arg2);  //проверка второго аргумента на соотвтетсвие существующему регистру

            *reg_arg = IsRegister(reg);
        }
    }

    else if (k == 0) // если нет второго аргумента
    {
        reg = cmdline + arg1_shift;
        *reg_arg = IsRegister(reg);  //проверка аргумента на соотвтетсвие существующему регистру
    }

    else  //если нет аргументов
        CompilationError(SYNTAX_ERR_CODE);

    // *num = num1 + num2;
    // *arg = *num + *reg_code;
}
