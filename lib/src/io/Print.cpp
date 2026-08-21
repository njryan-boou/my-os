#include <io/Print.hpp>
#include <terminal/Terminal.hpp>

namespace io {

void print(const char* text)
{
    kernel::Terminal terminal;
    terminal.write(text);
}

void print(char character)
{
    kernel::Terminal terminal;
    terminal.write(character);
}

void println(const char* text)
{
    kernel::Terminal terminal;

    terminal.write(text);
    terminal.write('\n');
}

void print_hex(std::uint64_t value)
{
    kernel::Terminal terminal;
    terminal.write_hex(value);
}

void backspace()
{
    kernel::Terminal terminal;
    terminal.backspace();

}

void tab()
{
    kernel::Terminal terminal;
    terminal.tab();
}

}