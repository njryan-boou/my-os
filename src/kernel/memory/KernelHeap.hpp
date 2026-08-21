#pragma once

#include <cstddef>
#include <cstdint>

namespace kernel::memory {

class PhysicalAllocator;

class KernelHeap
{
public:
    explicit KernelHeap(PhysicalAllocator& allocator);

    [[nodiscard]]
    void* allocate(std::size_t size);

private:
    static constexpr std::uint64_t heap_base = 0x50000000;
    static constexpr std::size_t page_size = 4096;

    PhysicalAllocator& allocator_;

    std::uint64_t current_virtual_ = heap_base;
    std::uint64_t current_page_end_ = heap_base;

    bool map_next_page();
};

}