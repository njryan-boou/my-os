#include "interrupts/IDT.hpp"
#include "memory/KernelHeap.hpp"
#include "memory/PhysicalAllocator.hpp"
#include "terminal/Terminal.hpp"

#include <cstdint>

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    kernel::interrupts::initialize();

    kernel::memory::PhysicalAllocator allocator;
    kernel::memory::KernelHeap heap(allocator);

    auto* a =
        static_cast<std::uint64_t*>(
            heap.allocate(sizeof(std::uint64_t)));

    auto* b =
        static_cast<std::uint64_t*>(
            heap.allocate(sizeof(std::uint64_t)));

    if (a == nullptr || b == nullptr)
    {
        terminal.write("Heap allocation failed.\n");

        for (;;)
        {
            asm volatile("hlt");
        }
    }

    *a = 0x1111222233334444ULL;
    *b = 0xAAAABBBBCCCCDDDDULL;

    terminal.write("A address: ");
    terminal.write_hex(
        reinterpret_cast<std::uint64_t>(a));
    terminal.write("\n");

    terminal.write("A value:   ");
    terminal.write_hex(*a);
    terminal.write("\n");

    terminal.write("B address: ");
    terminal.write_hex(
        reinterpret_cast<std::uint64_t>(b));
    terminal.write("\n");

    terminal.write("B value:   ");
    terminal.write_hex(*b);
    terminal.write("\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}