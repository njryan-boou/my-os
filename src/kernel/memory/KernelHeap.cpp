#include "KernelHeap.hpp"

#include "Paging.hpp"
#include "PhysicalAllocator.hpp"

namespace kernel::memory {

KernelHeap::KernelHeap(PhysicalAllocator& allocator)
    : allocator_(allocator)
{
}

bool KernelHeap::map_next_page()
{
    const std::uint64_t physical =
        allocator_.allocate();

    if (physical == 0)
    {
        return false;
    }

    const std::uint64_t virtual_address =
        current_page_end_;

    if (!Paging::map(
            virtual_address,
            physical,
            allocator_))
    {
        return false;
    }

    current_page_end_ += page_size;

    return true;
}

void* KernelHeap::allocate(std::size_t size)
{
    if (size == 0)
    {
        return nullptr;
    }

    // Simple 8-byte alignment.
    const std::size_t aligned_size =
        (size + 7) & ~static_cast<std::size_t>(7);

    while (current_virtual_ + aligned_size > current_page_end_)
    {
        if (!map_next_page())
        {
            return nullptr;
        }
    }

    void* result =
        reinterpret_cast<void*>(current_virtual_);

    current_virtual_ += aligned_size;

    return result;
}

}