#include "PhysicalAllocator.hpp"

#include "MemoryMap.hpp"

namespace kernel::memory {

std::uint64_t PhysicalAllocator::align_up(std::uint64_t address)
{
    return (address + page_size - 1) & ~(page_size - 1);
}

std::uint64_t PhysicalAllocator::allocate()
{
    constexpr std::uint64_t minimum_address = 0x100000;
    constexpr std::uint64_t maximum_address = 0x200000;

    for (std::size_t i = 0; i < MemoryMap::count(); ++i)
    {
        const MemoryRegion& region = MemoryMap::region(i);

        // E820 type 1 = usable RAM.
        if (region.type != 1)
        {
            continue;
        }

        std::uint64_t start = region.base;
        std::uint64_t end = region.base + region.length;

        if (start < minimum_address)
        {
            start = minimum_address;
        }

        if (end > maximum_address)
        {
            end = maximum_address;
        }

        start = align_up(start);

        if (next_ < start)
        {
            next_ = start;
        }

        if (next_ >= end)
        {
            continue;
        }

        if (end - next_ < page_size)
        {
            continue;
        }

        const std::uint64_t page = next_;

        next_ += page_size;

        return page;
    }

    return 0;
}

}