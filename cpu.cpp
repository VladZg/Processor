#include "Stack/Config.h"
#include "Constants.h"
#include <stdio.h>
#include <string.h>
#include "Stack/Stack.h"
#include "DumpGraphics.h"
#include "TechInfo.h"

#undef  FILENAME_INPUT
#define FILENAME_INPUT "output.asm"

int main()
{
    FILE* file = fopen(FILENAME_INPUT, "rb");
    ASSERT(file != NULL);

    // int* tech_info = (int*) calloc(TECH_INFO_SIZE, sizeof(int));
    // ASSERT(tech_info != NULL);

    TechInfo tech_info = {};

    fread(&tech_info, sizeof(int), TECH_INFO_SIZE, file);

    if ((tech_info.filecode == CP_FILECODE) && (tech_info.version == CMD_VERSION))
    {
        int code_size = tech_info.code_size;

        int* code = (int*) calloc(code_size, sizeof(int));
        ASSERT(code != NULL);

        fread(code, sizeof(int), code_size, file);

        Stack stk = {};
        StackCtor(stk);

        int ip = 0;

        while (ip < code_size)
        {
            switch(code[ip])
            {
                case CMD_PUSH:
                {
                    StackPush(&stk, code[1 + ip++]);
                    SimpleStackDump(&stk, "Push");

                    break;
                }

                case CMD_ADD:
                {
                    StackPush(&stk, StackPop(&stk) + StackPop(&stk));
                    SimpleStackDump(&stk, "Add");

                    break;
                }

                case CMD_SUB:
                {
                    int num = StackPop(&stk);

                    StackPush(&stk, StackPop(&stk) - num);
                    SimpleStackDump(&stk, "Sub");

                    break;
                }

                case CMD_MUL:
                {
                    StackPush(&stk, StackPop(&stk) * StackPop(&stk));
                    SimpleStackDump(&stk, "Mul");

                    break;
                }

                case CMD_DIV:
                {
                    int num = StackPop(&stk);

                    StackPush(&stk, StackPop(&stk) / num);
                    SimpleStackDump(&stk, "Div");

                    break;
                }

                case CMD_OUT:
                {
                    int popped = StackPop(&stk);
                    SimpleStackDump(&stk, "Out");
                    fprintf(stderr, "  OUTPUT: %d\n", popped);

                    break;
                }

                case CMD_PIN:
                {
                    int num = 0;

                    fprintf(stderr, "Type a number, it'll be used in calculatings: ");
                    scanf("%d", &num);

                    StackPush(&stk, num);
                    SimpleStackDump(&stk, "Pin");

                    break;
                }

                case CMD_HLT:
                {
                    StackDtor(&stk);
                    fprintf(stderr, "\n  Program \"%s\" has finished correctly\n", FILENAME_INPUT);
                    exit(1);

                    break;
                }

                case CMD_DUMP:
                {
                    FullDump(code, code_size, ip, &stk);

                    break;
                }

                default:
                {
                    fprintf(stderr, "NO SUCH COMMAND WITH CODE %d\n", code[ip]);
                    exit(1);
                }
            }

            ip++;
        }

        free(code);
        // free(tech_info);
        fclose(file);
    }

    else if (tech_info.filecode != CP_FILECODE)
    {
        fprintf(stderr, "WRONG TYPE OF ASM FILE!!!\nYOU HAVE TO USE CP_FILECODE \"%d\"\n", CP_FILECODE);
        // free(tech_info);
        fclose(file);
        exit(1);
    }

    else if (tech_info.version != CMD_VERSION)
    {
        fprintf(stderr, "USED OLD (\"%d\") VERSION OF COMMANDS!!!\nYOU HAVE TO USE THE \"%d\" VERSION!!!\n",
                tech_info.version, CMD_VERSION);
        // free(tech_info);
        fclose(file);
        exit(1);
    }

    return 0;
}
