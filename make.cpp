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

    struct stat buff_source, buff_asm, buff_cpu, buff_disasm, buff_source_out;

    size_t source_chng_time = 0, asm_chng_time = 0, cpu_chng_time = 0, disasm_chng_time = 0, source_out_chng_time = 0;
    int    is_file_changed  = 0;

    fread(&asm_chng_time,        sizeof(size_t), 1, filesinfo_file);
    fread(&cpu_chng_time,        sizeof(size_t), 1, filesinfo_file);
    fread(&disasm_chng_time,     sizeof(size_t), 1, filesinfo_file);
    fread(&source_chng_time,     sizeof(size_t), 1, filesinfo_file);
    fread(&source_out_chng_time, sizeof(size_t), 1, filesinfo_file);

    fseek(filesinfo_file, 0, SEEK_SET);

    stat(ASM_FILE,       &buff_asm);
    stat(CPU_FILE,       &buff_cpu);
    stat(DISASM_FILE,    &buff_disasm);
    stat(SOURCE_FILE,    &buff_source);
    stat(SOURCE_FILE,    &buff_source_out);

    fwrite(&buff_asm.st_mtime,        sizeof(size_t), 1, filesinfo_file);
    fwrite(&buff_cpu.st_mtime,        sizeof(size_t), 1, filesinfo_file);
    fwrite(&buff_disasm.st_mtime,     sizeof(size_t), 1, filesinfo_file);
    fwrite(&buff_source.st_mtime,     sizeof(size_t), 1, filesinfo_file);
    fwrite(&buff_source_out.st_mtime, sizeof(size_t), 1, filesinfo_file);

    WriteCompilationInfo("\"%s\":\n", __FILE__);

    if (asm_chng_time - buff_asm.st_mtime)  // перекомпиляция "asm.cpp", если файл изменился
    {
        system("g++ asm.cpp Onegin/functions.cpp -o asm");
        WriteCompilationInfo("  File \"%s\" was recompiled\n", ASM_FILE);
        is_file_changed++;
    }

    if (cpu_chng_time - buff_cpu.st_mtime)  // перекомпиляция "cpu.cpp", если файл изменился
    {
        system("g++ cpu.cpp Stack/Stack.cpp Stack/HashCounters.cpp Stack/Log.cpp Onegin/functions.cpp -o cpu");
        WriteCompilationInfo("  File \"%s\" was recompiled\n", CPU_FILE);
        is_file_changed++;
    }

    if (disasm_chng_time - buff_disasm.st_mtime)  // перекомпиляция "disasm.cpp" если файл изменился
    {
        system("g++ disasm.cpp -o disasm");
        WriteCompilationInfo("  File \"%s\" was recompiled\n", DISASM_FILE);
        is_file_changed++;
    }

    if (source_chng_time - buff_source.st_mtime)  // перекомпиляция, если файл "Source.txt" изменился
    {
        system("./asm " SOURCE_FILE);
        WriteCompilationInfo("  File \"%s\" was recompiled\n", SOURCE_FILE);
        is_file_changed++;
    }

    if (source_out_chng_time - buff_source_out.st_mtime)  // перекомпиляция, если файл "Source.txt" изменился
    {
        system("./disasm " SOURCE_ASM_FILE);
        WriteCompilationInfo("  File \"%s\" was redisassemblered\n", SOURCE_ASM_FILE);
        is_file_changed++;
    }

    fclose(filesinfo_file);

    if (!(is_file_changed))  // проверка изменился ли "FilesInfo.txt"
        WriteCompilationInfo("  No one file was recompiled\n", SOURCE_FILE);

    WriteCompilationInfo("\n");

    WriteCompilationInfo("\"cpu\":\n");

    system("./cpu " SOURCE_ASM_FILE);

    return 0;
}
