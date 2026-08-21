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

        if (row_ >= height_)
        {
            scroll();
        }

        return;
    }

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

        if (row_ >= height_)
        {
            scroll();
        }
    }
}

void Terminal::scroll()
{
    // Copy rows 1-24 into rows 0-23.
    for (std::size_t row = 1; row < height_; ++row)
    {
        for (std::size_t column = 0; column < width_; ++column)
        {
            const std::size_t source =
                row * width_ + column;

            const std::size_t destination =
                (row - 1) * width_ + column;

            buffer_[destination] = buffer_[source];
        }
    }

    // Clear the new bottom row.
    const std::size_t last_row =
        (height_ - 1) * width_;

    for (std::size_t column = 0; column < width_; ++column)
    {
        buffer_[last_row + column] = blank_;
    }

    // Continue writing on the bottom row.
    row_ = height_ - 1;
}

}