#ifndef CONSTS
#define CONSTS_H

#include "ColourConsts.h"

enum {
      STK_OK_STATUS        = 1,
      STK_DEL_STATUS       = 2,
      STK_N_INIT_STATUS    = 4,
      STK_ERR_STATUS       = 8,
      STK_ALR_INIT_STATUS  = 16,
      STK_DAMAGED_STATUS   = 32,

      STK_N_INIT_ERR       = 1,
      STK_N_EXIST_ERR      = 2,
      STK_ALR_INIT_ERR     = 4,
      STK_IS_NULL_ERR      = 8,
      STK_OVERFLOW_ERR     = 16,
      STK_UNDERFLOW_ERR    = 32,
      CHANGED_CANARY_ERR   = 64,
      STK_DATA_IS_NULL_ERR = 128,
      MASK_SHIFT           = 1,

      STK_INIT_MASK        = 254,
      STK_DEL_MASK         = 253,

      UNDEFINED_LINE       = -1,
      MIN_CAPACITY         = 10,
      HASH_SEED            = 0,

      PUSH_RESIZE_KEY      = 1,
      POP_RESIZE_KEY       = 0
     };

const  int   int_poisoned        = 0xBADDED;
const  char  char_ptr_poisoned[] = "poisoned";
const  char  char_poisoned       = '\?';
const  float float_poisoned      = 13.37;

typedef const int canary_t;

canary_t LEFT_CANARY  = 0xBCBCBC;
canary_t RIGHT_CANARY = 0xCBCBCB;

static char  undefined_stk_name[]    = KRED "?" KNRM;

#define UNDEFINED_STK_NAME undefined_stk_name
// #define LOG_FILE_NAME "Log1.txt"

#endif
