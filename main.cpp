#include "stdlib.h"

int main()
{
    system("g++ asm.cpp Onegin/functions.cpp -o asm");
    system("g++ disasm.cpp -o disasm");
    system("g++ cpu.cpp Stack/Stack.cpp Stack/Log.cpp Stack/HashCounters.cpp Onegin/functions.cpp -o cpu");
    system("g++ make.cpp Stack/Log.cpp -o make");

    system("./asm "    "Source.txt");
    // system("./disasm " "Source_output.asm");
    system("./cpu "    "Source_output.asm");

    return 0;
}
