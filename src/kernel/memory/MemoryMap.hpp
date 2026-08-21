#pragma once

#include <cstddef>
#include <cstdint>

namespace kernel::memory {

struct [[gnu::packed]] MemoryRegion
{
    std::uint64_t base;
    std::uint64_t length;
    std::uint32_t type;
    std::uint32_t attributes;
};

class MemoryMap
{
public:
    [[nodiscard]]
    static std::size_t count();

    [[nodiscard]]
    static const MemoryRegion& region(std::size_t index);
};

}