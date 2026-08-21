#pragma once

#include <cstddef>

namespace kernel {

class Terminal
{
public:
    void write(const char* text);

private:
    volatile unsigned short* buffer_ =
        reinterpret_cast<volatile unsigned short*>(0xB8000);

    std::size_t position_ = 0;
};

}