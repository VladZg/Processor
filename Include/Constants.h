#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEF_CMD(name, num, ...)             \
            CMD_##name = num,

#define DEF_JMP(name, num, condition, ...)  \
            DEF_CMD(name, num)

#define DEF_DUMP(name, num, ...)            \
            DEF_CMD(name, num)

enum Comands      //автогенерация enuma команд
{
      #include "./Cmd.h"
};

#undef DEF_DUMP
#undef DEF_JMP
#undef DEF_CMD

enum ArgsInfo
{
      NUMBER    = 1,
      REGISTER  = 2,
      RAM_ELEM  = 3,

      ARG_IMMED = 32,
      ARG_REG   = 64,
      ARG_MEM   = 128,
};

enum ErrCodes
{
      SYNTAX_ERR_CODE      = 1,
      DIV_ON_ZERO_ERR_CODE = 2,
      ARG_TYPE_ERR_CODE    = 3,
      ARG_ERR_CODE         = 4,
      CMD_ERR_CODE         = 5,
      LBL_ERR_CODE         = 6,
};

int START_REGS[7] = {0, 0, 0, 0, 0, 0, 0};

#define DEF_REG( name, num )              \
      REG_##name = num,                   \

enum RegistersInfo
{
      #include "./Reg.h"

      REGS_SIZE = 6,
};

#undef DEF_REG

enum RAMInfo
{
      RAM_WIDTH  = 81,
      RAM_HEIGTH = 21,
      RAM_SIZE   = RAM_WIDTH * RAM_HEIGTH,
      RAM_POISON = 0xBADDED,
};

enum LabelsInfo
{
      LBLS_MAXSIZE     = 100,
      LBLNAME_MAXSIZE  = 20,
};

enum OtherInfo
{
      CP_FILECODE      = 1337,
      CMD_VERSION      = 5,
      WRNG_CMD_VERSION = -1,

      TECH_INFO_SIZE   = 3,

      CMD_CODE_MASK    = 31,
};

const char  LBL_POISON_NAME[LBLNAME_MAXSIZE] = "LABEL POISON";
const int   LBL_POISON_VALUE                 = 0xBAD;

// DelaysInfo
const float GET_RAM_DELAY    = 1.0;
const float JUMP_DELAY       = 1.0;
const float BETW_STEPS_DELAY = 0.5;
const float MIN_CYCLE_DELAY  = 0.5;
const float PRINT_RAM_DELAY  = 0.0;

#endif
