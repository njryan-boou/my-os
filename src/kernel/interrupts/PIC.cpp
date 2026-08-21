#include <interrupts/PIC.hpp>

#include <cstdint>

namespace {

void outb(
    std::uint16_t port,
    std::uint8_t value)
{
    asm volatile(
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port));
}

void io_wait()
{
    outb(0x80, 0);
}

}

namespace kernel::interrupts {

void initialize_pic()
{
    constexpr std::uint16_t master_command = 0x20;
    constexpr std::uint16_t master_data = 0x21;

    constexpr std::uint16_t slave_command = 0xA0;
    constexpr std::uint16_t slave_data = 0xA1;

    outb(master_command, 0x11);
    io_wait();

    outb(slave_command, 0x11);
    io_wait();

    // Master IRQs -> 0x20 - 0x27
    outb(master_data, 0x20);
    io_wait();

    // Slave IRQs -> 0x28 - 0x2F
    outb(slave_data, 0x28);
    io_wait();

    // Slave connected to master's IRQ 2.
    outb(master_data, 0x04);
    io_wait();

    outb(slave_data, 0x02);
    io_wait();

    // 8086 mode.
    outb(master_data, 0x01);
    io_wait();

    outb(slave_data, 0x01);
    io_wait();

    // Enable only IRQ 1: keyboard.
    outb(master_data, 0xFD);

    // Disable all slave IRQs.
    outb(slave_data, 0xFF);
}

void send_eoi()
{
    outb(0x20, 0x20);
}

}
