#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "CheckFile.h"

// #define PROGS_DIRECTORY     "SimpleProgs/"
#define DEFAULT_DIRECTORY   ""

#ifdef  PROGS_DIRECTORY
#define DIRECTORY       PROGS_DIRECTORY
#else
#define DIRECTORY       DEFAULT_DIRECTORY
#endif

#define SOURCE_FILE_NAME    "Source"

#define SOURCE_FILE_DEFAULT "Source_default.txt"
#define SOURCE_FILE         DIRECTORY SOURCE_FILE_NAME ".txt"
#define SOURCE_ASM_FILE     "Source_output.asm"
#define ASM_FILE            "asm.cpp"
#define CPU_FILE            "cpu.cpp"
#define DISASM_FILE         "disasm.cpp"
#define FILESINFO_FILE      "FilesInfo.txt"

const char* source_file = nullptr;

// #define N_WR_COMP_INFO

FILE* filesinfo_file = fopen(FILESINFO_FILE, "rb+");

#ifndef N_WR_COMP_INFO

void WriteCompilationInfo(const char* format_data, ...)
{
    va_list args;
    va_start(args, format_data);

    vfprintf(stderr, format_data, args);

    va_end(args);
}

#else

void WriteCompilationInfo(const char* format_data, ...) {}

#endif

int main(const int argc, const char** argv)
{
    if (!CheckFile(argc, argv, &source_file))
        source_file = SOURCE_FILE_DEFAULT;

    int is_file_changed  = 0;

    size_t* buff_in = (size_t*) calloc(sizeof(size_t), 4);
    struct stat* buff = (struct stat*) calloc(sizeof(struct stat), 4);

    fread(buff_in, sizeof(size_t), 4, filesinfo_file);
    fseek(filesinfo_file, 0, SEEK_SET);

    stat(ASM_FILE,       buff);
    stat(CPU_FILE,       buff + 1);
    stat(DISASM_FILE,    buff + 2);
    stat(SOURCE_FILE,    buff + 3);

    size_t* buff_out = (size_t*) calloc(sizeof(size_t), 4);

    for (int i = 0; i < 5; i++)
        buff_out[i] = buff[i].st_mtime;

    fwrite(buff_out, sizeof(size_t), 4, filesinfo_file);

    WriteCompilationInfo("\"%s\":\n", __FILE__);

    if (buff_in[0] - buff_out[0])  // перекомпиляция "asm.cpp", если файл изменился
    {
        system("g++ asm.cpp Onegin/functions.cpp -o asm");
        WriteCompilationInfo("  File \"%s\" was recompiled\n", ASM_FILE);
        is_file_changed++;
    }

    if (buff_in[1] - buff_out[1])  // перекомпиляция "cpu.cpp", если файл изменился
    {
        system("g++ cpu.cpp Stack/Stack.cpp Stack/HashCounters.cpp Stack/Log.cpp Onegin/functions.cpp -o cpu");
        WriteCompilationInfo("  File \"%s\" was recompiled\n", CPU_FILE);
        is_file_changed++;
    }

    if (buff_in[2] - buff_out[2])  // перекомпиляция "disasm.cpp" если файл изменился
    {
        system("g++ disasm.cpp -o disasm");
        WriteCompilationInfo("  File \"%s\" was recompiled\n", DISASM_FILE);
        is_file_changed++;
    }

    if (buff_in[3] - buff_out[3])  // перекомпиляция, если файл "Source.txt" изменился
    {
        system("./asm " SOURCE_FILE);
        WriteCompilationInfo("  File \"%s\" was recompiled\n", SOURCE_FILE);
        system("./disasm " SOURCE_ASM_FILE);
        WriteCompilationInfo("  File \"%s\" was redisassemblered\n", SOURCE_ASM_FILE);
        is_file_changed += 2;
    }

    fclose(filesinfo_file);

    if (!(is_file_changed))  // проверка изменился ли "FilesInfo.txt"
        WriteCompilationInfo("  No one file was recompiled\n", SOURCE_FILE);

    WriteCompilationInfo("\n");

    WriteCompilationInfo("\"cpu\":\n");

    system("./cpu " SOURCE_ASM_FILE);

    free(buff_in);
    free(buff);
    free(buff_out);

    return 0;
}
