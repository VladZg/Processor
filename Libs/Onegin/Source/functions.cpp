#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>

// #define NDEBUG
#include "../Include/defines.h"


int size_of_file(FILE* file)                                  // узнать размер файла
{
    ASSERT(file != NULL);

    struct stat buf = {};
    fstat(fileno(file), &buf);
    return buf.st_size;
}

int read_file_to_data(FILE* file, char** data, int file_size) // считывание файла в строку
{
    ASSERT(file != NULL);
    ASSERT(data != NULL);

    *data = (char*) calloc(file_size, sizeof(char));
    ASSERT(*data != NULL);

    return fread(*data, sizeof(char), file_size, file);
}

void write_text_in_file(FILE* file, char** text)              // запись массива указателей в файл
{
    ASSERT(file != NULL);
    ASSERT(text != NULL);

    for (int i = 0; text[i] != NULL;)
    {
        fputs(text[i++], file);
        fputs("\n", file);
    }
}


void fill_text(char* data, char** text, int data_length) // заполнение массива указателей на строки
{
    ASSERT(data != NULL);
    ASSERT(text != NULL);

    int str_length = 0, i = 0, j = 0;

    while (i++ < data_length)
    {
        if (*data == '\n')
        {
            *data       = '\0';
            data       -= str_length;
            text[j++]   = data;
            data       += str_length;
            str_length  = 0;
        }
        else
            str_length++;

        data++;
    }

    text[j] = nullptr;
}


int symb_count(char symb, char* string) // подсчёт кол-ва заданных символов в строке
{
    ASSERT(string != NULL);

    int count = 0;

    while (*string != '\0')
    {
        if (*string++ == symb)
            count++;
    }

    return count;
}

static int is_letter_or_0(char symb)    // проверка на небукву или \0
{
    return isalpha(symb) || (symb == '\0');
}


static void skip_non_letters(const char** string1, const char** string2, const int direction)               // пропускание первых небукв
{
    ASSERT(string1 != NULL);
    ASSERT(string2 != NULL);

    int flag1 = is_letter_or_0(**string1), flag2 = is_letter_or_0(**string2);

    while (!(flag1 && flag2))
    {
        if (!flag1)
            *string1 += direction;
            flag1 = is_letter_or_0(*((*string1)));

        if (!flag2)
            *string2 += direction;
            flag2 = is_letter_or_0(*(*string2));
    }
}

static int strcmp_letters_only(const char** string1, const char** string2, const int direction)             // сравнение строк без знаков пунктуации
{
    ASSERT(string1 != NULL);
    ASSERT(string2 != NULL);

    if (direction == -1)
    {
        *string1 += strlen(*string1) - 1;
        *string2 += strlen(*string2) - 1;
    }

    skip_non_letters(string1, string2, direction);

    int i = 0;

    while((*string1)[i] == (*string2)[i])
    {
        if ((*string1)[i] == '\0')
            return 0;

        i += direction;
    }

    return (*string1)[i] - (*string2)[i];
}

static int cmp(const void*  str1_ptr, const void*  str2_ptr, const int direction,
               int (*strcmp_letters_only)(const char** string1, const char** string2, const int direction)) // общая функция сравнения void* строк
{
    ASSERT(str1_ptr            != NULL);
    ASSERT(str2_ptr            != NULL);
    ASSERT(strcmp_letters_only != NULL);

    const char *string1 = *(const char**) str1_ptr;
    const char *string2 = *(const char**) str2_ptr;

    return strcmp_letters_only(&string1, &string2, direction);
}

int cmp_left_to_right(const void* str1_ptr, const void* str2_ptr)                                           // функция сравнения слева-направо
{
    ASSERT(str1_ptr != NULL);
    ASSERT(str2_ptr != NULL);

    return cmp(str1_ptr, str2_ptr, 1, strcmp_letters_only);
}

int cmp_right_to_left(const void* str1_ptr, const void* str2_ptr)                                           // функция сравнения справа-налево
{
    ASSERT(str1_ptr != NULL);
    ASSERT(str2_ptr != NULL);

    return cmp(str1_ptr, str2_ptr, -1, strcmp_letters_only);
}


void buble_sort(char** text, size_t text_lines_amount, int size, int (*cmp)(const void* str1, const void* str2)) // собственная сортировка (пузырьком)
{
    ASSERT(text != NULL);
    ASSERT(cmp  != NULL);

    for (int i = (int) text_lines_amount; i >= 0; i--)
    {
        for (int j = 0; j < i-1; j++)
        {
            if (cmp(text + j, text + (j+1)) >= 0)
            {
                // swap(text[j], text[j+1]);
                char* temp = text[j];
                text[j]    = text[j+1];
                text[j+1]  = temp;
            }
        }
    }
}


void fputs_original(FILE* file, char* data, int data_length) //печать в файл исходного текста
{
    ASSERT(file != NULL);
    ASSERT(data != NULL);

    for (int i = 0; i < data_length; i++)
    {
        if (data[i] != '\0')
            fputc(data[i], file);

        else
            fputc('\n', file);
    }
}


void read_file_to_text(const char* filename_inp, char** data, char*** text, int* text_lines_amount)
{
    FILE* fileinp = fopen(filename_inp, "r");

    int file_size   = size_of_file(fileinp);
    int data_length = read_file_to_data(fileinp, data, file_size);

    fclose(fileinp);

    *text_lines_amount = symb_count('\n', *data);

    *text = (char**) calloc(*text_lines_amount + 1, sizeof(char *));
    ASSERT(*text != NULL);

    fill_text(*data, *text, data_length);
}
