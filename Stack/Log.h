#ifndef LOG_H
#define LOG_H

#include "Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "time.h"

FILE* OpenLog  ();
void  WriteLog (const char* format_data, ...);
void  CloseLog ();

#endif
