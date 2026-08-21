#include "interrupts/IDT.hpp"
#include "memory/Paging.hpp"
#include "memory/PhysicalAllocator.hpp"
#include "terminal/Terminal.hpp"

#include <cstdint>

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    kernel::interrupts::initialize();

    kernel::memory::PhysicalAllocator allocator;

    const std::uint64_t physical =
        allocator.allocate();

    constexpr std::uint64_t virtual_address =
        0x40000000;

    kernel::memory::Paging::map(
        virtual_address,
        physical);

    terminal.write("Physical: ");
    terminal.write_hex(physical);
    terminal.write("\n");

    terminal.write("Virtual:  ");
    terminal.write_hex(virtual_address);
    terminal.write("\n");

    /*
     * Write through the NEW virtual address.
     */
    volatile auto* value =
        reinterpret_cast<volatile std::uint64_t*>(
            virtual_address);

    *value = 0x123456789ABCDEF0ULL;

    /*
     * Since the physical page is also inside our identity-mapped
     * first GiB, we can read the same RAM through its physical
     * address too.
     */
    volatile auto* physical_view =
        reinterpret_cast<volatile std::uint64_t*>(
            physical);

    terminal.write("Value:    ");
    terminal.write_hex(*physical_view);
    terminal.write("\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}