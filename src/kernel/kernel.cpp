#include "interrupts/IDT.hpp"
#include "memory/MemoryMap.hpp"
#include "terminal/Terminal.hpp"

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    kernel::interrupts::initialize();

    terminal.write("Physical memory map\n\n");

    const std::size_t count =
        kernel::memory::MemoryMap::count();

    for (std::size_t i = 0; i < count; ++i)
    {
        const auto& region =
            kernel::memory::MemoryMap::region(i);

        terminal.write("Region ");
        terminal.write_hex(i);
        terminal.write("\n");

        terminal.write("  Base:   ");
        terminal.write_hex(region.base);
        terminal.write("\n");

        terminal.write("  Length: ");
        terminal.write_hex(region.length);
        terminal.write("\n");

        terminal.write("  Type:   ");
        terminal.write_hex(region.type);
        terminal.write("\n\n");
    }

    for (;;)
    {
        asm volatile("hlt");
    }
}