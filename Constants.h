#ifndef CONSTANTS_H
#define CONSTANTS_H

enum {
      CMD_HLT          = 0,
      CMD_PUSH         = 1,
      CMD_ADD          = 2,
      CMD_SUB          = 3,
      CMD_MUL          = 4,
      CMD_DIV          = 5,
      CMD_OUT          = 6,
      CMD_DUMP         = 7,
      CMD_PIN          = 8,
      CMD_POP          = 9,

      WRNG_CMD         = -1,
      WRNG_CMD_VERSION = -1,

      RAM_SIZE         = 100,

      SYNTAX_ERR_CODE      = 1,
      DIV_ON_ZERO_ERR_CODE = 2,

      CP_FILECODE      = 1337,
      CMD_VERSION      = 4,

      TECH_INFO_SIZE   = 3,

      RAX_CODE         = 1,
      RBX_CODE         = 2,
      RCX_CODE         = 3,
      RDX_CODE         = 4,

      ARG_IMMED        = 16,
      ARG_REG          = 32,
      ARG_MEM          = 64,

      CMD_CODE_MASK    = 15
     };

const float GET_RAM_DELAY = 0.5;

#endif
