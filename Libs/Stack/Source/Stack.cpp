#include "../Include/Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../Include/HashCounters.h"
#include "../Include/Defines.h"
#include "../Include/Stack.h"


#ifndef N_HASH_PROTECTION

void SetStackHash(Stack* stack)
{
    ASSERT(stack != NULL);

    stack->hash = 0;
    // stack->hash = MurHash((const void*) (stack - sizeof(canary_t)), (uint32_t) (sizeof(Stack) - 2 * sizeof(canary_t)), HASH_SEED);
    stack->hash = MurHash((const void*) stack, (uint32_t) sizeof(Stack), HASH_SEED);
}

void CheckStackHash(Stack* stack)
{
    ASSERT(stack != NULL);

    if (stack->status == STK_N_INIT_STATUS)
        SetStackHash(stack);

    uint32_t hash_temp = stack->hash;

    SetStackHash(stack);

    if (stack->hash - hash_temp)
    {
        stack->status = STK_DAMAGED_STATUS;

        WriteLog(KYEL "STACK IS DAMAGED\n" KNRM);
        StackDecodeErrors(StackError(stack));
        StackDump_(stack);

        exit(1);
    }
}

#else

void SetStackHash(Stack* stack) {}
void CheckStackHash(Stack* stack) {}

#endif


#ifndef N_CANARY_PROTECTION

size_t CheckCanaries(Stack* stack)
{
    return !((stack->left_canary - LEFT_CANARY)  | (stack->right_canary - RIGHT_CANARY));
}

#else

size_t CheckCanaries(Stack* stack)
{
    return 1;
}

#endif


void print_stack_elem(int elem)
{
    WriteLog("%d", elem);
}

void print_stack_elem(float elem)
{
    WriteLog("%f", elem);
}

void print_stack_elem(char elem)
{
    WriteLog("%c", elem);
}

void print_stack_elem(char* elem)
{
    WriteLog("%s", elem);
}


void poisoned(int* elem)
{
    int tmp_int_poisoned = (int) int_poisoned;
    *elem = tmp_int_poisoned;
}

void poisoned(float* elem)
{
    float tmp_float_poisoned = (float) float_poisoned;
    *elem = tmp_float_poisoned;
}

void poisoned(char* elem)
{
    char tmp_char_poisoned = (char) char_poisoned;
    *elem = tmp_char_poisoned;
}

void poisoned(char** elem)
{
    char* tmp_char_ptr_poisoned = (char*) char_ptr_poisoned;
    *elem = tmp_char_ptr_poisoned;
}


size_t StackError(Stack* stack)
{
    ASSERT(stack != NULL);

    size_t err = 0;

    if (stack != NULL)
    {
        if (stack->status == STK_N_INIT_STATUS)
        {
            err |= STK_N_INIT_ERR;
            return err;
        }

        if (stack->status == STK_DEL_STATUS)
        {
            err |= STK_N_EXIST_ERR;
            return err;
        }

        if (stack->status == STK_ALR_INIT_STATUS)
        {
            err |= STK_ALR_INIT_ERR;
            return err;
        }

        if (stack == NULL)
            err |= STK_IS_NULL_ERR;

        if (stack->size > stack->capacity)
            err |= STK_OVERFLOW_ERR;

        if (stack->capacity < MIN_CAPACITY)
            err |= STK_UNDERFLOW_ERR;

        if (!CheckCanaries(stack))
            err |= CHANGED_CANARY_ERR;

        if (stack->data == nullptr)
            err |= STK_DATA_IS_NULL_ERR;

        if (err)
        {
            stack->status = STK_ERR_STATUS;
            SetStackHash(stack);
        }
    }

    return err;
}

void StackDecodeErrors(size_t error)
{
    #define StackPrintError( err_msg )                                          \
    {                                                                           \
        if (error & mask)                                                       \
            WriteLog(err_msg);                                                  \
                                                                                \
        mask <<= MASK_SHIFT;                                                    \
    }

    size_t mask = 1;

    StackPrintError(KYEL "STACK WASN'T INITIALIZED\n"                  KNRM);
    StackPrintError(KYEL "STACK DOESN'T EXIST (DELETED)\n"             KNRM);
    StackPrintError(KYEL "STACK WAS ALREADY INITIALIZED\n"             KNRM);
    StackPrintError(KYEL "STACK EQUALS NULL\n"                         KNRM);
    StackPrintError(KYEL "STACK OVERFLOW (SIZE > CAPACITY)\n"          KNRM);
    StackPrintError(KYEL "STACK UNDERFLOW (CAPACITY < MIN_CAPACITY)\n" KNRM);
    StackPrintError(KYEL "ONE OF STACK CANARIES WAS CHANGED\n"         KNRM);
    StackPrintError(KYEL "STACK DATA EQUALS NULL\n"                    KNRM);

    #undef StackPrintfError
}

void StackDecodeStatus(size_t status)
{
    if(status == STK_OK_STATUS)
        WriteLog("%s", KGRN "OK"                 KNRM);

    else if(status == STK_DEL_STATUS)
        WriteLog("%s", KGRN "DELETED"            KNRM);

    else if(status == STK_N_INIT_STATUS)
        WriteLog("%s", KGRN "NOT INITIALIZED"    KNRM);

    else if(status == STK_ERR_STATUS)
        WriteLog("%s", KGRN "ERROR"              KNRM);

    else if(status == STK_ALR_INIT_STATUS)
        WriteLog("%s", KGRN "ALREDY INITIALIZED" KNRM);

    else if(status == STK_DAMAGED_STATUS)
        WriteLog("%s", KGRN "DAMAGED"            KNRM);
}


void FillPoisons(Stack* stack, size_t start_index)
{
    ASSERT(stack != NULL);

    Elem_t poison = (Elem_t) NULL;
    poisoned(&poison);

    for (size_t i = start_index; i < stack->capacity; i++)
        stack->data[i] = poison;

    SetStackHash(stack);
}

void StackDump_(Stack* stack)
{
    WriteLog("Stack[%p] (", (void *) stack);
    StackDecodeStatus(stack->status);
    WriteLog(") \"%s\" at \"%s\"(%d)               \n", stack->info.name, stack->info.filename, stack->info.line);
    WriteLog("{                                    \n");
    WriteLog("    size     = %lu                   \n", stack->size);
    WriteLog("    capacity = %lu                   \n", stack->capacity);
    WriteLog("    data[%p]                         \n", (void *) &(stack->data));
    WriteLog("    {                                \n");

    Elem_t poison = (Elem_t) NULL;
    poisoned(&poison);

    if (stack->data)
    {
        for (size_t i = 0; i < stack->capacity; i++)

            if (stack->data[i] != poison)
            {
                WriteLog("        *[%ld] = " KMAG, i);
                print_stack_elem(stack->data[i]);
                WriteLog(KNRM "\n");
            }

            else
            {
                WriteLog("         [%ld] = " KBLU, i);
                print_stack_elem(stack->data[i]);
                WriteLog(" (poison)\n" KNRM);
            }
    }

    WriteLog("    }                                \n");
    WriteLog("}                                    \n\n");
}

void StackCtor_(Stack* stack)
{
    ASSERT (stack != NULL)

    SetStackHash(stack);

    if (stack->status == STK_OK_STATUS)
        stack->status = STK_ALR_INIT_STATUS;

    size_t err = StackError(stack);

    if (!(err & STK_INIT_MASK))
    {
        stack->data         = (Elem_t*) calloc(MIN_CAPACITY, sizeof(Elem_t));
        stack->capacity     = MIN_CAPACITY;
        stack->size         = (size_t) 0;
        stack->left_canary  = LEFT_CANARY;
        stack->right_canary = RIGHT_CANARY;

        Elem_t poison = (Elem_t) NULL;
        poisoned(&poison);

        FillPoisons(stack, 0);

        stack->status = STK_OK_STATUS;
    }

    else if (!(err & STK_DEL_MASK))
        stack->status = STK_N_INIT_STATUS;

    SetStackHash(stack);

    ASSERT_OK(stack);
}

void StackDtor(Stack* stack)
{
    ASSERT(stack != NULL);
    ASSERT_OK(stack);

    if (stack->status != STK_DEL_STATUS)
    {
        free(stack->data);
        stack->data     = (Elem_t*) NULL;
        stack->size     = 0;
        stack->capacity = 0;
        stack->status   = STK_DEL_STATUS;
    }

    SetStackHash(stack);
}

void StackRealloc(Stack* stack, size_t capacity)
{
    ASSERT(stack != NULL);

    stack->capacity = capacity;
    stack->data = (Elem_t*) realloc(stack->data, capacity * sizeof(Elem_t));

    ASSERT(stack->data != NULL);

    SetStackHash(stack);
}

void StackResize(Stack* stack, size_t key)
{
    if (key == PUSH_RESIZE_KEY)
    {
        StackRealloc(stack, stack->capacity * 2);
        FillPoisons(stack, stack->size + 1);
    }

    else if (key == POP_RESIZE_KEY)
    {
        size_t new_capacity = stack->capacity;

        if (stack->size-- == 1)
            new_capacity = MIN_CAPACITY;

        else if ((stack->capacity / stack->size >= 4) && (stack->capacity / 4 >= MIN_CAPACITY))
            new_capacity = stack->capacity / 4;

        StackRealloc(stack, new_capacity);
    }
}

Elem_t StackPop(Stack* stack, int* err)
{
    ASSERT_OK(stack);

    Elem_t popped = (Elem_t) NULL;

    if (stack->size > 0)
    {
        Elem_t poison = (Elem_t) NULL;
        poisoned(&poison);

        popped = stack->data[stack->size - 1];
        stack->data[stack->size - 1] = poison;

        StackResize(stack, POP_RESIZE_KEY);
    }

    else if (err != NULL)
        *err = 1;

    SetStackHash(stack);

    ASSERT_OK(stack);

    return popped;
}

void StackPush(Stack* stack, const Elem_t value)
{
    ASSERT_OK(stack);

    if (stack->size >= stack->capacity)
        StackResize(stack, PUSH_RESIZE_KEY);

    stack->data[stack->size++] = value;

    SetStackHash(stack);

    ASSERT_OK(stack);
}

void SimpleStackDump_(Stack* stack)
{
    Elem_t poison = (Elem_t) NULL;
    poisoned(&poison);

    if (stack->data)
    {
        if (stack->size > 0)
        {
            for (size_t i = 0; i < stack->capacity; i++)

                if (stack->data[i] != poison)
                {
                    fprintf(stderr, "    *[%ld] = " KMAG, i);
                    print_stack_elem(stack->data[i]);
                    fprintf(stderr, KNRM "\n");
                }

            #define N_WRITE_FULL_STACK
            #ifndef N_WRITE_FULL_STACK
                else
                {
                    fprintf(stderr, "     [%ld] = " KBLU, i);
                    print_stack_elem(stack->data[i]);
                    fprintf(stderr, " (poison)\n" KNRM);
                }
            #endif
        }

        else fprintf(stderr, KBLU "    Stack is empty\n" KNRM);

    }
}
