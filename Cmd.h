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
            StackPush(&STACK, *arg);

        else CpuError(&cpu, file, ARG_ERR_CODE, "ARGUMENT OF PUSH IS EMPTY (POISONED) ELEM FROM THE RAM");

        SimpleStackDump(&STACK);
    })

DEF_CMD (ADD,  2,  0,
    {
        StackPush(&STACK, StackPop(&STACK) + StackPop(&STACK));

        SimpleStackDump(&STACK);
    })

DEF_CMD (SUB,  3,  0,
    {
        int num = StackPop(&STACK);

        StackPush(&STACK, StackPop(&STACK) - num);

        SimpleStackDump(&STACK);
    })

DEF_CMD (MUL,  4,  0,
    {
        StackPush(&STACK, StackPop(&STACK) * StackPop(&STACK));

        SimpleStackDump(&STACK);
    })

DEF_CMD (DIV,  5,  0,
    {
        int num = StackPop(&STACK);

        if (num)
        {
            StackPush(&STACK, StackPop(&STACK) / num);

            SimpleStackDump(&STACK);
        }

        else CpuError(&cpu, file, DIV_ON_ZERO_ERR_CODE, "DIVISION ON ZERO");
    })

DEF_CMD (OUT,  6,  0,
    {
        int popped = StackPop(&STACK);

        SimpleStackDump(&STACK);

        fprintf(stderr, "    %d ", popped);

        if (popped == RAM_POISON) fprintf(stderr, KGRN "(RAM poison)" KNRM);

        fprintf(stderr, "\n\n");
    })

DEF_CMD (DUMP, 7,  1,
    {
        char ip_min = CODE[IP++];
        char ip_max = CODE[IP++];

        FullDump(&cpu, ip_min, ip_max);
    })

DEF_CMD (PIN,  8,  0,
    {
        int num = 0;

        fprintf(stderr, "  Type a number, it'll be used in calculatings: ");

        if (scanf("%d", &num))
        {
            StackPush(&STACK, num);

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

        *arg = StackPop(&STACK);

        SimpleStackDump(&STACK);
        RegsDump(cpu.Regs);
        RAMDump(cpu.RAM);
    })

DEF_JMP (JMP,  10, 1)

DEF_JMP (JB,   11, StackPop(&STACK) > StackPop(&STACK))

DEF_JMP (JBE,  12, StackPop(&STACK) >= StackPop(&STACK))

DEF_JMP (JA,   13, StackPop(&STACK) < StackPop(&STACK))

DEF_JMP (JAE,  14, StackPop(&STACK) <= StackPop(&STACK))

DEF_JMP (JE,   15, StackPop(&STACK) == StackPop(&STACK))

DEF_JMP (JNE,  16, StackPop(&STACK) != StackPop(&STACK))

DEF_JMP (JF,   17, IsFridayToday())
