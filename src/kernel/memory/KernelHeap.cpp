#include "KernelHeap.hpp"

#include "Paging.hpp"
#include "PhysicalAllocator.hpp"

namespace kernel::memory {

KernelHeap::KernelHeap(PhysicalAllocator& allocator)
    : allocator_(allocator)
{
}

std::size_t KernelHeap::align(std::size_t size)
{
    constexpr std::size_t alignment = 8;

    return (size + alignment - 1)
        & ~(alignment - 1);
}

bool KernelHeap::expand(std::size_t required_size)
{
    std::size_t total_size =
        required_size + sizeof(Block);

    std::size_t pages =
        (total_size + page_size - 1) / page_size;

    const std::uint64_t region_start = mapped_end_;

    for (std::size_t i = 0; i < pages; ++i)
    {
        const std::uint64_t physical =
            allocator_.allocate();

        if (physical == 0)
        {
            return false;
        }

        if (!Paging::map(
                mapped_end_,
                physical,
                allocator_))
        {
            return false;
        }

        mapped_end_ += page_size;
    }

    auto* block =
        reinterpret_cast<Block*>(region_start);

    block->size =
        pages * page_size - sizeof(Block);

    block->free = true;
    block->next = nullptr;

    if (head_ == nullptr)
    {
        head_ = block;
        return true;
    }

    Block* current = head_;

    while (current->next != nullptr)
    {
        current = current->next;
    }

    current->next = block;

    merge_free_blocks();

    return true;
}

void KernelHeap::split(
    Block* block,
    std::size_t size)
{
    constexpr std::size_t minimum_remaining = 8;

    if (block->size <
        size + sizeof(Block) + minimum_remaining)
    {
        return;
    }

    auto* new_block =
        reinterpret_cast<Block*>(
            reinterpret_cast<std::uintptr_t>(block)
            + sizeof(Block)
            + size);

    new_block->size =
        block->size - size - sizeof(Block);

    new_block->free = true;
    new_block->next = block->next;

    block->size = size;
    block->next = new_block;
}

void* KernelHeap::allocate(std::size_t size)
{
    if (size == 0)
    {
        return nullptr;
    }

    size = align(size);

    for (;;)
    {
        Block* current = head_;

        while (current != nullptr)
        {
            if (current->free &&
                current->size >= size)
            {
                split(current, size);

                current->free = false;

                return reinterpret_cast<void*>(
                    reinterpret_cast<std::uintptr_t>(current)
                    + sizeof(Block));
            }

            current = current->next;
        }

        if (!expand(size))
        {
            return nullptr;
        }
    }
}

void KernelHeap::free(void* pointer)
{
    if (pointer == nullptr)
    {
        return;
    }

    auto* block =
        reinterpret_cast<Block*>(
            reinterpret_cast<std::uintptr_t>(pointer)
            - sizeof(Block));

    block->free = true;

    merge_free_blocks();
}

void KernelHeap::merge_free_blocks()
{
    Block* current = head_;

    while (current != nullptr &&
           current->next != nullptr)
    {
        Block* next = current->next;

        const auto current_end =
            reinterpret_cast<std::uintptr_t>(current)
            + sizeof(Block)
            + current->size;

        if (current->free &&
            next->free &&
            current_end ==
                reinterpret_cast<std::uintptr_t>(next))
        {
            current->size +=
                sizeof(Block) + next->size;

            current->next = next->next;

            continue;
        }

        current = current->next;
    }
}

}