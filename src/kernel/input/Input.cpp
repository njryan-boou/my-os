#include "Input.hpp"

namespace {

constexpr std::size_t capacity = 256;

char buffer[capacity]{};

std::size_t length = 0;
bool ready = false;

}

namespace kernel::input {

void push(char character)
{
    if (ready)
    {
        return;
    }

    if (character == '\n')
    {
        buffer[length] = '\0';
        ready = true;
        return;
    }

    // Leave one byte available for '\0'.
    if (length >= capacity - 1)
    {
        return;
    }

    buffer[length] = character;
    ++length;

    buffer[length] = '\0';
}

void backspace()
{
    if (ready || length == 0)
    {
        return;
    }

    --length;
    buffer[length] = '\0';
}

bool line_ready()
{
    return ready;
}

const char* line()
{
    return buffer;
}

void clear()
{
    length = 0;
    ready = false;

    buffer[0] = '\0';
}

}