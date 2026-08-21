#pragma once

#include <cstdint>

namespace kernel::memory {

class PhysicalAllocator;

class Paging
{
public:
    static constexpr std::uint64_t page_size = 4096;

    static bool map(
        std::uint64_t virtual_address,
        std::uint64_t physical_address,
        PhysicalAllocator& allocator);
};

}