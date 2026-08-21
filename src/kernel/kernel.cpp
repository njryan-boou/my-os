#include <input/Input.hpp>
#include <interrupts/IDT.hpp>
#include <io/Print.hpp>
#include <terminal/Terminal.hpp>

extern "C" void kernel_main()
{
    kernel::Terminal terminal;
    terminal.clear();
    io::println("My OS");
    io::println("");

    kernel::interrupts::initialize();

    asm volatile("sti");

    io::print("> ");

    for (;;)
    {
        if (kernel::input::line_ready())
        {
            io::print("You entered: ");
            io::println(kernel::input::line());

            kernel::input::clear();

            io::print("> ");
        }

        asm volatile("hlt");
    }
}