#ifndef CONFIG_H
#define CONFIG_H

// #define RELEASE_MODE
#define NDEBUG
#define NLOGGING
// #define N_HASH_PROTECTION
// #define N_CANARY_PROTECTION

#ifdef RELEASE_MODE

    #define  NDEBUG
    #define  NLOGGING
    #define  N_HASH_PROTECTION
    #define  N_CANARY_PROTECTION

#endif

#include "./Consts.h"

typedef int Elem_t;

#endif
