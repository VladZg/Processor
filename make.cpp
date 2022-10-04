#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define SOURCE_FILE   "Source.txt"
#define SETTINGS_FILE "SourceFileInfo.txt"

FILE* settings_file = fopen(SETTINGS_FILE, "r+");

int main()
{
    struct stat buff;

    size_t file_changes_time = 0;
    fscanf(settings_file, "%ld", &file_changes_time);

    fseek(settings_file, 0, SEEK_SET);

    stat(SOURCE_FILE, &buff);
    fprintf(settings_file, "%ld\n", buff.st_mtime);

    if (file_changes_time - buff.st_mtime)  // перекомпиляция, если исходный файл изменился
        system("./asm " SOURCE_FILE);

    system("./cpu");


    return 0;
}
