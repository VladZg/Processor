#include "../Include/Constants.h"
#include "../Libs/Stack/Include/Config.h"
#include "../Libs/Stack/Include/ColourConsts.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <stdarg.h>
#include "../Libs/Onegin/Include/defines.h"
#include "../Libs/Onegin/Include/functions.h"
#include "../Include/TechInfo.h"
#include "../Include/CheckFile.h"
#include "../Include/Description.h"

#undef  FILENAME_INPUT
#undef  FILENAME_OUTPUT
#undef  FILENAME_INPUT_DEFAULT

const char  FILENAME_INPUT_DEFAULT[]  = "./AsmProgs/Source_default.asm";
const char* FILENAME_INPUT            = nullptr;
const char  FILENAME_OUTPUT[]         = "./Source.exe";
const char  FILENAME_LISTING[]        = "./Source.lst";

int    n_labels = 0;
Label* labels = nullptr;
int    compilations_amnt = 0;

int    Compile (const char* filename_input, const char* filename_output);
int    CompilationError(FILE* file, int err_code, const char* fmt_err_msg, ...);

void   LablesCtor(Label* labels);
void   LablesCleaner(Label* labels);
int    IsLabel(char* name);
int    LabelValue(char* name);
void   WriteLabels(FILE* file_listing);

size_t IsArgs (const char* cmdname, char* cmdline);
int    GetArg (char* cmdline, int* num_arg, int* reg_arg);
int    SetCodes (char* cmdline, char** code, int* code_size, int* ip, int Cmd_code);
int    SetJumpCode(char* cmdline, char** code, int* code_size, int* ip, int Cmd_code);

void   WriteListing(FILE* file, char* code, int ip, int n_args, size_t arg_size = sizeof(int));

// struct Cmd {
//             unsigned short mem    : 1;
//             unsigned short reg    : 1;
//             unsigned short immed  : 1;
//             unsigned short code   : 5;
//             };


int main(const int argc, const char** argv)
{
    PrintHelp(argc, argv);

    if (!CheckFile(argc, argv, &FILENAME_INPUT))
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;

    labels = (Label*) calloc(LBLS_MAXSIZE + 1, sizeof(Label));
    LablesCtor(labels);

    n_labels = 0;

    // PrintLabels();

    Compile(FILENAME_INPUT, FILENAME_OUTPUT);
    compilations_amnt++;

    // PrintLabels();

    Compile(FILENAME_INPUT, FILENAME_OUTPUT);

    LablesCleaner(labels);
    free(labels);

    return 0;
}


void WriteLabels(FILE* file_listing)
{
    fprintf(file_listing, "\nNamed labels\n{\n");
    for (int i = 1; i <= LBLS_MAXSIZE; i++)
    {
        if (strlen(labels[i].name) > 0)
            fprintf(file_listing, "    \"%20s\" = %d\n", labels[i].name, labels[i].value);
    }
    fprintf(file_listing, "}\n");
}

int CompilationError(FILE* file, int err_code, const char* fmt_err_msg, ...)
{
    ASSERT(file != NULL);
    ASSERT(fmt_err_msg != NULL);

    fprintf(stderr, KYEL "    ERROR (%d): \n    ", err_code);

    va_list args;
    va_start(args, fmt_err_msg);

    vfprintf(stderr, fmt_err_msg, args);
    fprintf(stderr, KNRM KMAG "\n\n    PROGRAM STOPPED!!!\n\n" KNRM);

    va_end(args);

    free(labels);
    fclose(file);

    // exit(1);
    return 0;
}

int IsLabel(char* name)
{
    for (int i = 1; i <= LBLS_MAXSIZE; i++)
    {
        if (!strcmp(labels[i].name, name))
            return i;
    }

    return 0;
}

#define DEF_REG( name, num )                \
    if (strcasecmp(str, #name) == 0)        \
        return REG_##name;                  \
    else

int IsRegister(char* str)
{
    #include "../Include/Reg.h"

    {
        // CompilationError(stderr, SYNTAX_ERR_CODE, "WRONG SYNTAX");
        fprintf(stderr, KYEL "    WRONG SYNTAX:\n"
                             "        NO SUCH REGISTER CALLED \"%s\"\n\n" KNRM, str);
        // exit(1);
        return 0;
    }

    return 0;
}

#undef DEF_REG

size_t IsArgs(const char* cmdname, char* cmdline)
{
    return strlen(cmdline) - strlen(cmdname);
}

int LabelValue(char* name)
{
    int index = IsLabel(name);

    if (index)
        return labels[index].value;

    else if (compilations_amnt)
    {
        // CompilationError(stderr, LBL_ERR_CODE, "NO SUCH LABEL CALLED \"%s\"", name);
        fprintf(stderr, KYEL "    NO SUCH LABEL CALLED \"%s\"\n\n" KNRM, name);

        // exit(1);
        return 0;
    }

    return 0;
}

void LablesCtor(Label* labels)
{
    for (int i = 1; i <= LBLS_MAXSIZE; i++)
    {
        labels[i].name = (char*) calloc(LBLNAME_MAXSIZE, sizeof(char));
        strcpy(labels[i].name, LBL_POISON_NAME);
        labels[i].value = LBL_POISON_VALUE;
    }
}

void LablesCleaner(Label* labels)
{
    for (int i = 0; i < LBLS_MAXSIZE; i++)
        if (labels[i].name)
        {
            free(labels[i].name);
            labels[i].value = 0;
        }
}

int GetArg(char* cmdline, int* num_arg, int* reg_arg)
{
    int arg1_shift = 0;   // arg1_shift - сдвиг первого аргумента отн начала строки
    int arg2_shift = 0;   // arg2_shift - 0, если нет второго аргумента, если есть - то это сдвиг от начала
    char* arg2 = nullptr;
    int num1 = 0, num2 = 0;
    char* reg = nullptr;

    int k = sscanf(cmdline, "%d + %n%*s", &num1, &arg2_shift); // k отвечает за то, есть ли 1й числовой аргумент

    if (k > 0) //если есть num1
    {
        if (arg2_shift) // если есть второй аргумент
        {
            arg2 = cmdline + arg2_shift;

            if (sscanf(arg2, "%d", &num2) == 1);  //если второй аргумент - число

            else
            {
                sscanf(arg2, " %[^]]s", arg2);
                *reg_arg = IsRegister(arg2);  //проверка второго аргумента на соотвтетсвие существующему регистру
            }
        }
    }

    else if (k == 0) // если нет второго аргумента
    {
        reg = cmdline;
        sscanf(reg, " %[^]]s ", reg);
        *reg_arg = IsRegister(reg);  //проверка аргумента на соотвтетсвие существующему регистру
    }

    else  //если нет аргументов
    {
        // CompilationError(stderr, SYNTAX_ERR_CODE, "WRONG SYNTAX");
        fprintf(stderr, KYEL "    WRONG SYNTAX:"
                             "        NO ARGUMNENTS IN COMMAND \"%s\"\n\n" KNRM, cmdline);
        // exit(1);
        return 0;
    }

    *num_arg = num1 + num2;
    return 1;
}

int SetCodes(char* cmdline, char** code, int* code_size, int* ip, int Cmd_code)
{
    int shift1 = 0, shift2 = 0;
    int is_mem_brack = 0;

    sscanf(cmdline, "%*s %n[%n", &shift1, &shift2);

    if ((shift2 - shift1) == 1)
    {
        is_mem_brack++;
        cmdline += shift2;
    }

    else
        cmdline += shift1;

    char cmd_code = 0 | Cmd_code;
    int  num_arg = 0;
    int  reg_arg = 0;

    GetArg(cmdline, &num_arg,  &reg_arg);

    if (is_mem_brack)
    {
        // int arg_len = strlen(cmdline);
        // fprintf(stderr, "%s\n", cmdline);
        // q1 = sscanf(cmdline, "%n]", &q2);
        // fprintf(stderr, "%s\n\n", cmdline + arg_len - 1);

        // if (*(cmdline + arg_len - 1) == ']')
        //  {
            // fprintf(stderr, "INSANE!\n");
            cmd_code |= ARG_MEM;  // всегда когда открывается скобочка
        // }

        // else
        // {
            // fprintf(stderr, "SYNTAX ERROR\n");
            // exit(1);
        // }
    }

    int n_args = 0;
    int is_immed = num_arg || !reg_arg;
    int is_reg   = reg_arg;

    if (is_immed)
    {
        cmd_code |= ARG_IMMED;
        n_args++;
    }

    if (is_reg)
    {
        cmd_code |= ARG_REG;
        n_args++;
    }

    // fprintf(stderr, "\n%d + %d = %d + %d + %d -> %d\n\n", num_arg, reg_arg, is_immed, is_reg, cmd_code & ARG_MEM, cmd_code);

    (*code)[(*ip)++] = cmd_code;

    if (is_immed)
    {
        // PutInt(*code + *ip, num_arg);
        *(int*)(*code + *ip) = num_arg;
        *ip += sizeof(int);
        *code_size += sizeof(int);
    }

    if (is_reg)
    {
        // PutInt(*code + *ip, reg_arg);
        *(int*)(*code + *ip) = reg_arg;
        *ip += sizeof(int);
        *code_size += sizeof(int);
    }

    return n_args;
}

int SetJumpCode(char* cmdline, char** code, int* code_size, int* ip, int Cmd_code)
{
    char cmd_code = 0 | Cmd_code;

    (*code)[(*ip)++] = cmd_code;

    // fprintf(stderr, "cmd_code = %d\n", cmd_code);

    int num = 0;
    int label_shift = 0;

    char* arg = (char*) calloc(LBLNAME_MAXSIZE, sizeof(char));
    sscanf(cmdline, "%*s :%n%s",  &label_shift, arg);

    if (label_shift)  //если аргумент - именная или числовая метка с :
    {
        // PutInt(*code + *ip, LabelValue(arg));
        *(int*)(*code + *ip) = LabelValue(arg);
        // fprintf(stderr, "метка %s -> %d\n", arg, LabelValue(arg));
    }

    else if (sscanf(cmdline, "%*s%d", &num)) //если аргумент - число
    {
        // PutInt(*code + *ip, num);
        *(int*)(*code + *ip) = num;
        // fprintf(stderr, "число %d\n", num);
    }

    else  //если аргумент - числовая метка без :
    {
        sscanf(cmdline, "%*s %s", arg);
        // PutInt(*code + *ip, LabelValue(arg));
        *(int*)(*code + *ip) = LabelValue(arg);
        // fprintf(stderr, "именная метка %s -> %d\n", arg, LabelValue(arg));
    }

    *ip += sizeof(int);
    *code_size += sizeof(int);

    // fprintf(stderr, "JUMP: \n\"%s\" -> \"%s\"\n", cmdline, arg);

    free(arg);

    return 1;
}

void WriteListing(FILE* file, char* code, int ip, int n_args, size_t arg_size)
{
    ip--;

    for (int i = 0; i < n_args; i++)
        ip -= arg_size;

    fprintf(file, "[%04d] ", ip);          //ip
    fprintf(file, "[%04d] ", code[ip++]);  //cmd_code

    for (int i = 0; i < n_args; i++)
    {
        if (arg_size == 4) fprintf(file, "[%08x] ", *(int*)(code + ip)); //int arguments

        else fprintf(file, "[%08x] ", code[ip]); //char arguments

        ip += arg_size;
    }

    fprintf(file, "\n");
}

#define DEF_CMD(name, num, arg, ...)                                                  \
if (strcasecmp(cmd, #name) == 0)                                                      \
{                                                                                     \
    int n_args = 0;                                                                   \
                                                                                      \
    if (arg)                                                                          \
        n_args = SetCodes(text[line], &code, &tech_info.code_size, &ip, CMD_##name);  \
                                                                                      \
    else                                                                              \
        code[ip++] = cmd_code | CMD_##name;                                           \
                                                                                      \
    WriteListing(file_listing, code, ip, n_args);                                     \
                                                                                      \
    tech_info.code_size++;                                                            \
}                                                                                     \
else

#define DEF_JMP(name, num, condition, ...)                                            \
if (strcasecmp(cmd, #name) == 0)                                                      \
{                                                                                     \
    cmd_code = SetJumpCode(text[line], &code, &tech_info.code_size, &ip, CMD_##name); \
                                                                                      \
    WriteListing(file_listing, code, ip, 1);                                          \
                                                                                      \
    tech_info.code_size++;                                                            \
}                                                                                     \
else

//особая функция с особыми переменными
#define DEF_DUMP(name, num)                                                           \
if (strcasecmp(cmd, #name) == 0)                                                      \
{                                                                                     \
    code[ip++] = cmd_code | CMD_##name;                                               \
                                                                                      \
    int ip_min = 0, ip_max = 0;                                                       \
    int is_arg = sscanf(text[line], "%*s %d %d", &ip_min, &ip_max);                   \
                                                                                      \
    if ((is_arg < 1) || (ip_min >= ip_max))                                           \
    {                                                                                 \
        ip_min = (char) -1;                                                           \
        ip_max = (char) -1;                                                           \
    }                                                                                 \
                                                                                      \
    code[ip++] = ip_min;                                                              \
    code[ip++] = ip_max;                                                              \
                                                                                      \
    tech_info.code_size += 3;                                                         \
                                                                                      \
    WriteListing(file_listing, code, ip, 2, sizeof(char));                            \
}                                                                                     \
else

int Compile(const char* filename_input, const char* filename_output)
{
    char** text = nullptr;
    char*  data = nullptr;
    int    text_lines_amount = 0;

    FILE* file_out = fopen(filename_output, "wb");
    ASSERT(file_out != NULL);

    FILE* file_listing = fopen(FILENAME_LISTING, "w");
    ASSERT(file_listing != NULL);

    fprintf(file_listing, "[ ip ] [cmd ] [  arg1  ] [  arg2  ]\n");

    read_file_to_text(filename_input, &data, &text, &text_lines_amount);

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

        if (strstr(cmd, "\\\\")) {}

        // оставить для красоты вводимого кода, в общем случае убрать
        else if (!(strcmp(cmd, "")) || (!strcmp(cmd, "{")) || (!strcmp(cmd, "}"))) {}
        // if (strcmp(cmd, "") == 0) {}

        else
        #include "../Include/Cmd.h"

        {
            char* arg = cmd;
            sscanf(cmd, " %[^:]s", arg);

            if (n_labels < LBLS_MAXSIZE)
            {
                if (!IsLabel(arg))
                {
                    strcpy(labels[n_labels + 1].name, arg);
                    labels[++n_labels].value = ip;
                }
            }

            else if (!compilations_amnt)
            {
                // CompilationError(stderr, CMD_ERR_CODE, "THERE IS NO COMMAND CALLED \"%s\" IN \"%d\" VERSION!!!\n"
                                                    //    "    COMPILED FILE WOULD BE DAMAGED!!!", cmd, CMD_VERSION);

                fprintf(stderr, "    THERE IS NO COMMAND CALLED \"%s\" IN \"%d\" VERSION!!!\n"
                                "    COMPILED FILE WOULD BE DAMAGED!!!\n\n", cmd, CMD_VERSION);

                tech_info.version = WRNG_CMD_VERSION;

                fwrite((const void*) &tech_info, sizeof(int), TECH_INFO_SIZE, file_out);

                free(cmd);
                // exit(1);
                return 0;
            }
        }

        free(cmd);
        line++;
    }

    fwrite((const void*) &tech_info, sizeof(int), TECH_INFO_SIZE, file_out);
    fwrite((const void*) code, sizeof(char), tech_info.code_size, file_out);

    WriteLabels(file_listing);

    fclose(file_out);
    fclose(file_listing);
    free(text);
    free(data);
    free(code);

    return 1;
}

#undef DEF_DUMP
#undef DEF_JMP
#undef DEF_CMD
