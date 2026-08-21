#include "interrupts/IDT.hpp"
#include "memory/MemoryMap.hpp"
#include "terminal/Terminal.hpp"

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();

    kernel::interrupts::initialize();

    terminal.write("Kernel started.\n");
    terminal.write("IDT loaded.\n");

    terminal.write("Memory map entries: ");
    terminal.write_hex(
        kernel::memory::MemoryMap::count());
    terminal.write("\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}