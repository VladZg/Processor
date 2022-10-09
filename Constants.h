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
      CMD_JUMP         = 10,
      CMD_JB           = 11,
      CMD_JBE          = 12,
      CMD_JA           = 13,
      CMD_JAE          = 14,
      CMD_JE           = 15,
      CMD_JNE          = 16,
      CMD_JF           = 17
      } Comands;

enum {
      NUMBER           = 1,
      REGISTER         = 2,
      RAM_ELEM         = 3
      } Arg_types;

enum {
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

      REGS_SIZE        = 4,

      ARG_IMMED        = 32,
      ARG_REG          = 64,
      ARG_MEM          = 128,

      CMD_CODE_MASK    = 31
     };

const float GET_RAM_DELAY = 0.5;

#endif
