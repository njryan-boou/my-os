#include "IDT.hpp"

#include "../terminal/Terminal.hpp"

#include <cstddef>
#include <cstdint>

namespace {

struct [[gnu::packed]] IDTEntry
{
    std::uint16_t offset_low;
    std::uint16_t selector;
    std::uint8_t ist;
    std::uint8_t attributes;
    std::uint16_t offset_middle;
    std::uint32_t offset_high;
    std::uint32_t reserved;
};

struct [[gnu::packed]] IDTDescriptor
{
    std::uint16_t limit;
    std::uint64_t base;
};

IDTEntry idt[256]{};

extern "C" void invalid_opcode_stub();

void set_entry(
    std::size_t vector,
    void (*handler)())
{
    const auto address =
        reinterpret_cast<std::uint64_t>(handler);

    idt[vector].offset_low =
        static_cast<std::uint16_t>(address & 0xFFFF);

    idt[vector].selector = 0x18;
    idt[vector].ist = 0;

    // Present | Ring 0 | Interrupt Gate
    idt[vector].attributes = 0x8E;

    idt[vector].offset_middle =
        static_cast<std::uint16_t>(
            (address >> 16) & 0xFFFF);

    idt[vector].offset_high =
        static_cast<std::uint32_t>(
            (address >> 32) & 0xFFFFFFFF);

    idt[vector].reserved = 0;
}

}

namespace kernel::interrupts {

void initialize()
{
    set_entry(6, invalid_opcode_stub);

    const IDTDescriptor descriptor{
        static_cast<std::uint16_t>(sizeof(idt) - 1),
        reinterpret_cast<std::uint64_t>(idt)
    };

    asm volatile(
        "lidt %0"
        :
        : "m"(descriptor)
    );
}

}

extern "C" [[noreturn]]
void invalid_opcode_handler()
{
    kernel::Terminal terminal;

    terminal.clear();
    terminal.write("KERNEL PANIC\n");
    terminal.write("Invalid Opcode (#UD)\n");
    terminal.write("Vector: 0x");
    terminal.write_hex(6);

    for (;;)
    {
        asm volatile("cli; hlt");
    }
}