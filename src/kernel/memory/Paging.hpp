#pragma once

#include <cstdint>

namespace kernel::memory {

class Paging
{
public:
    static constexpr std::uint64_t page_size = 4096;

    static void map(
        std::uint64_t virtual_address,
        std::uint64_t physical_address);
};

}