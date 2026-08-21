#pragma once

#include <cstddef>

namespace kernel::input {

void push(char character);
void backspace();

[[nodiscard]]
bool line_ready();

[[nodiscard]]
const char* line();

void clear();

}