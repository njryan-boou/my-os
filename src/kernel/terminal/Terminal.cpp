#include "Terminal.hpp"

namespace kernel {

void Terminal::write(const char* text)
{
    while (*text != '\0')
    {
        const unsigned char character =
            static_cast<unsigned char>(*text);

        buffer_[position_] =
            static_cast<unsigned short>(0x0F00 | character);

        ++position_;
        ++text;
    }
}

}