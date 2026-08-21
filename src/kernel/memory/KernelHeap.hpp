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

    [[nodiscard]]
    void* allocate(
        std::size_t size,
        std::size_t alignment);

    void free(void* pointer);

private:
    struct Block
    {
        std::size_t size;
        bool free;
        Block* next;
    };

    static constexpr std::uint64_t heap_base = 0x50000000;
    static constexpr std::size_t page_size = 4096;

    PhysicalAllocator& allocator_;

    Block* head_ = nullptr;
    std::uint64_t mapped_end_ = heap_base;

    static std::size_t align_up(
        std::size_t value,
        std::size_t alignment);

    bool expand(std::size_t required_size);

    void split(
        Block* block,
        std::size_t consumed_size);

    void merge_free_blocks();
};

}