#pragma once

#include <cstddef>

namespace kernel::memory {

class MemoryMap
{
public:
    [[nodiscard]]
    static std::size_t count();
};

}