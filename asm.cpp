#include "Constants.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "Onegin/defines.h"
#include "Onegin/functions.h"
#include "TechInfo.h"

#undef  FILENAME_INPUT
#undef  FILENAME_OUTPUT
#undef  FILENAME_INPUT_DEFAULT

const char  FILENAME_OUTPUT[]        = "output.asm";
const char  FILENAME_INPUT_DEFAULT[] = "Source_default";
const char* FILENAME_INPUT           = nullptr;

int main(int argc, char** argv)
{
    if (argc == 2)
        FILENAME_INPUT = argv[1];

    else
        FILENAME_INPUT = FILENAME_INPUT_DEFAULT;

    char** text = nullptr;
    char*  data = nullptr;
    int text_lines_amount = 0;

    FILE* file_out = fopen(FILENAME_OUTPUT, "wb");
    ASSERT(file_out != NULL);

    read_file_to_text(FILENAME_INPUT, &data, &text, &text_lines_amount);

    int* code      = (int*) calloc(2 * text_lines_amount, sizeof(int));
    ASSERT(code != NULL);

    // int* tech_info = (int*) calloc(TECH_INFO_SIZE, sizeof(int));
    // ASSERT(tech_info != NULL);

    // sscanf(text[0], " %d %d %d", &(tech_info[0]), &(tech_info[1]), &(tech_info[2]));

    int line  = 0;
    int i     = 0;
    int count = 0;

    while (line < text_lines_amount)
    {
        char* cmd = (char*) calloc(strlen(text[line]), sizeof(char));
        ASSERT(cmd != NULL);

        sscanf(text[line], " %s", cmd);

        if (strcasecmp(cmd, "push") == 0)
        {
            int val = 0;
            sscanf(text[line] + 4, "%d", &val);
            code[i++] = CMD_PUSH;
            count++;
            code[i++] = val;
        }

        else if (strcasecmp(cmd, "add") == 0)
            code[i++] = CMD_ADD;

        else if (strcasecmp(cmd, "sub") == 0)
            code[i++] = CMD_SUB;

        else if (strcasecmp(cmd, "mul") == 0)
            code[i++] = CMD_MUL;

        else if (strcasecmp(cmd, "div") == 0)
            code[i++] = CMD_DIV;

        else if (strcasecmp(cmd, "out") == 0)
            code[i++] = CMD_OUT;

        else if (strcasecmp(cmd, "pin") == 0)
            code[i++] = CMD_PIN;

        else if (strcasecmp(cmd, "hlt") == 0)
            code[i++] = CMD_HLT;

        else if (strcasecmp(cmd, "dump") == 0)
            code[i++] = CMD_DUMP;

        else
        {
            fprintf(stderr, "NO COMMAND CALLED \"%s\"!!!\n", cmd);
            exit(1);
        }

        count++;
        free(cmd);
        line++;
    }

    TechInfo tech_info = {
                          CP_FILECODE,
                          CMD_VERSION,
                          count
                         };
    // tech_info[0] = CP_FILECODE;
    // tech_info[1] = CMD_VERSION;
    // tech_info[2] = count;

    fwrite((const void*) &tech_info, sizeof(int), TECH_INFO_SIZE, file_out);
    fwrite((const void*) code, sizeof(int), count, file_out);

    fclose(file_out);
    free(text);
    free(data);
    // free(tech_info);

    return 0;
}

