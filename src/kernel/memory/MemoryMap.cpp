#include "MemoryMap.hpp"

#include <cstdint>

namespace kernel::memory {

namespace {

constexpr std::uintptr_t count_address = 0x4FF0;
constexpr std::uintptr_t map_address   = 0x5000;

}

std::size_t MemoryMap::count()
{
    const volatile auto* count =
        reinterpret_cast<const volatile std::uint16_t*>(
            count_address);

    return *count;
}

const MemoryRegion& MemoryMap::region(std::size_t index)
{
    const auto* regions =
        reinterpret_cast<const MemoryRegion*>(map_address);

    return regions[index];
}

}