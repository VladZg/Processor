#include "Config.h"
#include <stdio.h>
#include <stdlib.h>
#include "Log.h"
#include "Defines.h"
#include "Stack.h"

#include "TestingValues.h"
#include "TestingFunctions.h"

int main()
{
    Stack stk1 = {};
    Stack stk2 = {};

    // StackDump(&stk1);
    // StackDump(&stk2);

    StackCtor(stk2);
    StackCtor(stk1);
    // StackCtor(stk1);

    // StackPushN(&stk1, 11);
    // stk1.left_canary = 9;
    // StackDump(&stk1);
    stk1.capacity = 3;
    stk1.size = 6;
    // stk1.data = NULL;


    StackPushN(&stk1, 41);

    // StackPopN(&stk1, 0);

    // stk1.size = 3;
    // StackPopN(&stk1, 56);
    // StackPush(&stk1, TEST_CHAR_PTR);
    // FillStack(&stk1);
    // stk1.data = NULL;
    // StackPush(&stk1, TEST_CHAR_PTR);
    // StackDump(&stk1);

    StackDtor(&stk1);
    StackDtor(&stk2);

    // StackCtor(stk1);
    // StackDump(&stk1);
    // StackDump(&stk2);

    // StackPush(&stk1, 2);
    // StackDtor(&stk1);

    return 0;
}
