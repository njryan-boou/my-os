#pragma once

#include <cstdint>

namespace io {

void print(const char* text);
void print(char character);
void println(const char* text);
void print_hex(std::uint64_t value);
void backspace();
void tab();

}