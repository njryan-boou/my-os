#pragma once

#include <cstddef>
#include <cstdint>

namespace kernel {

class Terminal
{
public:
    void clear();

    void write(const char* text);
    void write_hex(std::uint64_t value);

private:
    static constexpr std::size_t width_ = 80;
    static constexpr std::size_t height_ = 25;

    volatile std::uint16_t* buffer_ =
        reinterpret_cast<volatile std::uint16_t*>(0xB8000);

    std::size_t row_ = 0;
    std::size_t column_ = 0;

    void put_char(char character);
};

}