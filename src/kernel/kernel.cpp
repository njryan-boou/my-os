#include "interrupts/IDT.hpp"
#include "memory/PhysicalAllocator.hpp"
#include "terminal/Terminal.hpp"

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    kernel::interrupts::initialize();

    kernel::memory::PhysicalAllocator allocator;

    terminal.write("Physical page allocator\n\n");

    const auto page1 = allocator.allocate();
    const auto page2 = allocator.allocate();
    const auto page3 = allocator.allocate();

    terminal.write("Page 1: ");
    terminal.write_hex(page1);
    terminal.write("\n");

    terminal.write("Page 2: ");
    terminal.write_hex(page2);
    terminal.write("\n");

    terminal.write("Page 3: ");
    terminal.write_hex(page3);
    terminal.write("\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}