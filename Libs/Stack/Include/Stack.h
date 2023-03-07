#ifndef STACK_H
#define STACK_H

#include "./Config.h"
#include "./Log.h"
#include "./HashCounters.h"
#include "./Defines.h"

struct Info
{
    char*       name     = UNDEFINED_STK_NAME;
    const char* filename = __FILE__;
    int         line     = UNDEFINED_LINE;
};

struct Stack
{
    size_t   left_canary;

    Elem_t*  data;
    size_t   size;
    size_t   capacity;

    Info     info;
    size_t   status = STK_N_INIT_STATUS;
    uint32_t hash;

    size_t   right_canary;
};

void   print_stack_elem  (int   elem);
void   print_stack_elem  (float elem);
void   print_stack_elem  (char  elem);
void   print_stack_elem  (char* elem);

void   poisoned          (int*   elem);
void   poisoned          (float* elem);
void   poisoned          (char*  elem);
void   poisoned          (char** elem);

void   SetStackHash      (Stack* stack);
void   CheckStackHash    (Stack* stack);
size_t CheckCanaries     (Stack* stack);
size_t StackError        (Stack* stack);
void   StackDecodeErrors (size_t error);
void   StackDecodeStatus (size_t status);

void   FillPoisons       (Stack* stack, size_t start_index);
void   StackDump_        (Stack* stack);
void   StackCtor_        (Stack* stack);
void   StackDtor         (Stack* stack);
void   StackRealloc      (Stack* stack, size_t capacity);
void   StackResize       (Stack* stack, size_t key);
Elem_t StackPop          (Stack* stack, int* err = NULL);
void   StackPush         (Stack* stack, const Elem_t value);

void SimpleStackDump_    (Stack* stack);

#endif
