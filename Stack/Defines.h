#ifndef DEFINES_H
#define DEFINES_H

#include "Config.h"
#include <stdlib.h>
#include "Log.h"

# ifndef NDEBUG

    #define PRINT( cmd )                                                        \
    {                                                                           \
        WriteLog("\"%s\" in line %d in file \"%s\" in %s\n",                    \
                #cmd, __LINE__, __FILE__, __PRETTY_FUNCTION__);                 \
        cmd;                                                                    \
    }

    #define  ASSERT( condition )                                                \
    {                                                                           \
        if (!(condition))                                                       \
            WriteLog("Error in '%s' in line %d in file \"%s\" in %s\n",         \
                    #condition, __LINE__, __FILE__, __PRETTY_FUNCTION__);       \
    }


    #define ASSERT_OK( stack_ptr )                                              \
    {                                                                           \
        CheckStackHash(stack_ptr);                                              \
                                                                                \
        size_t err = StackError(stack_ptr);                                     \
                                                                                \
        if (err)                                                                \
        {                                                                       \
            WriteLog(KRED "ERROR " KNRM "in \"%s\" at \"%s\"(%d)\n",            \
                     __PRETTY_FUNCTION__, __FILE__, __LINE__);                  \
            StackDecodeErrors(err);                                             \
            StackDump(stack_ptr);                                               \
            exit(1);                                                            \
        }                                                                       \
                                                                                \
        else                                                                    \
            stack_ptr->status = STK_OK_STATUS;                                  \
                                                                                \
        SetStackHash(stack_ptr);                                                \
    }

#else
    #define ASSERT(    condition ) {}
    #define ASSERT_OK( stack )     {}
    #define PRINT(     cmd )       {}

#endif

#define StackDump( stack_ptr )                                                  \
        {                                                                       \
            WriteLog(KCYN "\"void StackDump(Stack* stack)\" in "                \
                          "\"%s\" at \"%s\" (%d):\n" KNRM,                      \
                     __PRETTY_FUNCTION__, __FILE__, __LINE__);                  \
            if (stack_ptr != NULL)                                              \
                StackDump_(stack_ptr);                                          \
                                                                                \
            else                                                                \
            {                                                                   \
                WriteLog(KYEL "POINTER TO STACK EQUALS NULL\n" KNRM);           \
                exit(1);                                                        \
            }                                                                   \
        }

#define StackCtor( stack )                                                      \
        {                                                                       \
            StackCtor_(&stack);                                                 \
            stack.info = {(char*) #stack, __FILE__, (size_t) __LINE__};         \
            SetStackHash(&stack);                                               \
        }

#endif
