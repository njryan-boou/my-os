#include "terminal/Terminal.hpp"

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();

    terminal.write("Hello from C++!\n");
    terminal.write("Kernel running in 64-bit mode.\n");

    terminal.write("Test address: ");
    terminal.write_hex(0xDEADBEEF);
}