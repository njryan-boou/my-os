#include "MemoryMap.hpp"

#include <cstdint>

namespace kernel::memory {

std::size_t MemoryMap::count()
{
    constexpr std::uintptr_t count_address = 0x4FF0;

    const volatile auto* count =
        reinterpret_cast<const volatile std::uint16_t*>(
            count_address);

    return *count;
}

}