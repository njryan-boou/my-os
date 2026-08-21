#include "Terminal.hpp"

namespace kernel {

void Terminal::clear()
{
    for (std::size_t i = 0; i < width_ * height_; ++i)
    {
        buffer_[i] = blank_;
    }

    row_ = 0;
    column_ = 0;
}

void Terminal::write(const char* text)
{
    while (*text != '\0')
    {
        put_char(*text);
        ++text;
    }
}

void Terminal::write_hex(std::uint64_t value)
{
    static constexpr char digits[] =
        "0123456789ABCDEF";

    write("0x");

    for (int shift = 60; shift >= 0; shift -= 4)
    {
        const std::uint64_t digit =
            (value >> shift) & 0xF;

        put_char(digits[digit]);
    }
}

void Terminal::put_char(char character)
{
    if (character == '\n')
    {
        column_ = 0;
        ++row_;
    }
    else
    {
        const std::size_t index =
            row_ * width_ + column_;

        buffer_[index] =
            static_cast<std::uint16_t>(
                attribute_ |
                static_cast<unsigned char>(character));

        ++column_;

        if (column_ >= width_)
        {
            column_ = 0;
            ++row_;
        }
    }

    // For now, simply wrap to the top.
    if (row_ >= height_)
    {
        row_ = 0;
    }
}

}