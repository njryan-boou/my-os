#include "interrupts/IDT.hpp"
#include "terminal/Terminal.hpp"

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    terminal.write("Kernel started.\n");

    kernel::interrupts::initialize();

    terminal.write("IDT loaded.\n");
    terminal.write("Triggering exception...\n");


    terminal.write("ERROR: returned from exception.\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}