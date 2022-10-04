#include "stdlib.h"

int main()
{
    system("g++ asm.cpp Onegin/functions.cpp -o asm");
    // system("g++ disasm.cpp -o disasm");
    system("g++ cpu.cpp Stack/Stack.cpp Stack/Log.cpp Stack/HashCounters.cpp Onegin/functions.cpp -o cpu");
    system("g++ make.cpp -o make");
    system("./make");

    return 0;
}
