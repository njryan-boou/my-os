#include "interrupts/IDT.hpp"
#include "terminal/Terminal.hpp"

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    terminal.write("Kernel started.\n");

    kernel::interrupts::initialize();

    terminal.write("IDT loaded.\n");
    terminal.write("Triggering invalid opcode...\n");

    asm volatile("ud2");

    // We should never reach this.
    terminal.write("ERROR: exception returned.\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}