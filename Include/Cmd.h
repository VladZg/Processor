#include "./DSL.h"

DEF_CMD (HLT,  0,  0,
    {
        fprintf(stderr, "    Program \"%s\" has finished correctly\n", FILENAME_INPUT);
        CpuCleaner(&cpu);
        fclose(file);

        return 1;
    })

DEF_CMD (PUSH, 1,  1,
    {
        int arg_temp = 0;
        int* arg = GetArg(&cpu, cmd, &arg_temp);

        if (*arg != RAM_POISON)
            S_PUSH(*arg);

        else CpuError(&cpu, file, ARG_ERR_CODE, "ARGUMENT OF PUSH IS EMPTY (POISONED) ELEM FROM THE RAM");

        SimpleStackDump(&STACK);
    })

DEF_CMD (ADD,  2,  0,
    {
        S_PUSH(S_POP + S_POP);

        SimpleStackDump(&STACK);
    })

DEF_CMD (SUB,  3,  0,
    {
        int num = S_POP;

        S_PUSH(S_POP - num);

        SimpleStackDump(&STACK);
    })

DEF_CMD (MUL,  4,  0,
    {
        S_PUSH(S_POP * S_POP);

        SimpleStackDump(&STACK);
    })

DEF_CMD (DIV,  5,  0,
    {
        int num = S_POP;

        if (num)
        {
            S_PUSH(S_POP / num);

            SimpleStackDump(&STACK);
        }

        else CpuError(&cpu, file, DIV_ON_ZERO_ERR_CODE, "DIVISION ON ZERO");
    })

DEF_CMD (OUT,  6,  0,
    {
        int popped = S_POP;

        SimpleStackDump(&STACK);

        fprintf(stderr, "    %d ", popped);

        if (popped == RAM_POISON) fprintf(stderr, KGRN "(RAM poison)" KNRM);

        fprintf(stderr, "\n\n");
    })

DEF_DUMP (DUMP, 7)

DEF_CMD (PIN,  8,  0,
    {
        int num = 0;

        fprintf(stderr, "  Type a number, it'll be used in calculatings: ");

        if (scanf("%d", &num))
        {
            S_PUSH(num);

            SimpleStackDump(&STACK);
        }

        else CpuError(&cpu, file, ARG_TYPE_ERR_CODE, "ARGUMENT OF PIN HAS TO BE AN INTEGER");
    })

DEF_CMD (POP,  9,  1,
    {
        int arg_temp = 0;
        int* arg = 0;

        if (cmd & ARG_MEM)
        {
            cmd &= ~ARG_MEM;
            arg = GetArg(&cpu, cmd, &arg_temp);

            if ((*arg <= RAM_SIZE) && (*arg >= 0))
                arg = GetRAM(&cpu, *arg);

            else CpuError(&cpu, file, ARG_ERR_CODE, "ARGUMENT OF POP (%d) IS OUT OF RAM RANGE", *arg);
        }

        else if ((cmd & ARG_REG) && !(cmd & ARG_IMMED))
        {
            arg = &cpu.Regs[CODE[IP]];
            IP += sizeof(int);
        }

        else CpuError(&cpu, file, ARG_TYPE_ERR_CODE, "WRONG TYPE OF POP ARGUMENT");

        *arg = S_POP;

        SimpleStackDump(&STACK);
        RegsDump(cpu.Regs);
        RAMDump(cpu.RAM);
    })

DEF_JMP (JMP,  10, 1)

DEF_JMP (JB,   11, S_POP >  S_POP)

DEF_JMP (JBE,  12, S_POP >= S_POP)

DEF_JMP (JA,   13, S_POP <  S_POP)

DEF_JMP (JAE,  14, S_POP <= S_POP)

DEF_JMP (JE,   15, S_POP == S_POP)

DEF_JMP (JNE,  16, S_POP != S_POP)

DEF_JMP (JF,   17, IsFridayToday())


//добавленные функции (ниже)

DEF_CMD (RUS, 18, 0,
    {
        int RUSSIA = PrintRusFlag();
        fprintf(stderr, "  MEOW\n\n");
    })

DEF_CMD (PRINT, 19, 1,
    {
        int arg_temp = 0;
        int* arg = GetArg(&cpu, cmd, &arg_temp);
        fprintf(stderr, "    ARGUMENT: %d ", *arg);

        if (*arg == RAM_POISON)
            fprintf(stderr, "(RAM POISONED)");

        fprintf(stderr, "\n\n");
    })

DEF_CMD (RAM, 20, 0,
    {
        fprintf(stderr, "  ");

        for (int j = 0; j < RAM_HEIGTH; j++)
        {
            for (int i = 1; i <= RAM_WIDTH; i++)
            {
                int num = cpu.RAM[RAM_WIDTH * j + i];

                if (num == RAM_POISON) fprintf(stderr, "□");

                else fprintf(stderr, KYEL "▩" KNRM);
            }

            fprintf(stderr, "\n  ");
        }

        fprintf(stderr, "\n\n");
    })

DEF_JMP (CALL, 21, 1,
    {
        StackPush(&cpu.stack_addr_ret, IP + sizeof(int));
    })

DEF_CMD (RET, 22, 0,
    {
        IP = StackPop(&cpu.stack_addr_ret);
    })

DEF_CMD (SIN, 23, 1,
    {
        int arg_temp = 0;
        int* arg = GetArg(&cpu, cmd, &arg_temp);

        // fprintf(stderr, "ARG: %f", ((float) (*arg)) / 10000000);
        S_PUSH(rintf((sinf(((float) (*arg)) / 10000000) * 10000000)));
        // fprintf(stderr, "\nSIN: %f\n", (sinf(((float) (*arg)) / 10000000)));

        SimpleStackDump(&STACK);
    })

DEF_CMD (COS, 24, 1,
    {
        int arg_temp = 0;
        int* arg = GetArg(&cpu, cmd, &arg_temp);

        // fprintf(stderr, "ARG: %f", ((float) (*arg)) / 10000000);
        S_PUSH(rintf((cosf(((float) (*arg)) / 10000000)) * 10000000));
        // fprintf(stderr, "\nCOS: %f\n", (cosf(((float) (*arg)) / 10000000)));

        SimpleStackDump(&STACK);
    })

DEF_CMD(CLEAR_CONSOLE, 25, 0,
    {
        sleep(PRINT_RAM_DELAY);
        system("clear");

        // for (int i = 0; i < 10000; i++)
        // {
        //     fprintf(stderr, "\r");
        // }
    })

DEF_CMD(CLEAR_RAM, 26, 0,
    {
        for (int i = 1; i <= RAM_SIZE; i++)
            cpu.RAM[i] = RAM_POISON;
    })

DEF_CMD(SQUARE, 27, 0,
    {
        S_PUSH(sqrt(S_POP));
    })

DEF_CMD(DELAY, 28, 1,
    {
        int arg_temp = 0;
        int* arg = GetArg(&cpu, cmd, &arg_temp);

        struct timespec t_r = {0, (*arg) * 1000000};
        struct timespec t_w;

        nanosleep(&t_r, &t_w);
    })

DEF_CMD(OUT_FLOAT, 29, 0,
    {
        fprintf(stderr, "    %.5f\n", S_POP / 1000.0);
    })
