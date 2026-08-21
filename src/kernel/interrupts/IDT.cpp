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


extern "C" void exception_0();
extern "C" void exception_6();
extern "C" void exception_8();
extern "C" void exception_13();
extern "C" void exception_14();


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

    // Present | Ring 0 | 64-bit interrupt gate
    idt[vector].attributes = 0x8E;

    idt[vector].offset_middle =
        static_cast<std::uint16_t>(
            (address >> 16) & 0xFFFF);

    idt[vector].offset_high =
        static_cast<std::uint32_t>(
            (address >> 32) & 0xFFFFFFFF);

    idt[vector].reserved = 0;
}


const char* exception_name(std::uint64_t vector)
{
    switch (vector)
    {
        case 0:
            return "Divide Error";

        case 6:
            return "Invalid Opcode";

        case 8:
            return "Double Fault";

        case 13:
            return "General Protection Fault";

        case 14:
            return "Page Fault";

        default:
            return "Unknown Exception";
    }
}

}


namespace kernel::interrupts {

void initialize()
{
    set_entry(0, exception_0);
    set_entry(6, exception_6);
    set_entry(8, exception_8);
    set_entry(13, exception_13);
    set_entry(14, exception_14);

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
void exception_handler(
    std::uint64_t vector,
    std::uint64_t error_code)
{
    kernel::Terminal terminal;

    terminal.clear();

    terminal.write("KERNEL PANIC\n\n");

    terminal.write("Exception: ");
    terminal.write(exception_name(vector));
    terminal.write("\n");

    terminal.write("Vector:    ");
    terminal.write_hex(vector);
    terminal.write("\n");

    terminal.write("Error:     ");
    terminal.write_hex(error_code);
    terminal.write("\n");

    if (vector == 14)
    {
        std::uint64_t fault_address;

        asm volatile(
            "mov %%cr2, %0"
            : "=r"(fault_address)
        );

        terminal.write("Address:   ");
        terminal.write_hex(fault_address);
        terminal.write("\n");
    }

    for (;;)
    {
        asm volatile("cli; hlt");
    }
}