#pragma once

#include <cstdint>

namespace kernel::memory {

class PhysicalAllocator
{
public:
    static constexpr std::uint64_t page_size = 4096;

    [[nodiscard]]
    std::uint64_t allocate();

private:
    std::uint64_t next_ = 0x100000;

    static std::uint64_t align_up(std::uint64_t address);
};

}