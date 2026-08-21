#include <memory/KernelHeap.hpp>

#include <memory/Paging.hpp>
#include <memory/PhysicalAllocator.hpp>

#include <cstddef>
#include <cstdint>

namespace kernel::memory {

KernelHeap::KernelHeap(PhysicalAllocator& allocator)
    : allocator_(allocator)
{
}

std::size_t KernelHeap::align_up(
    std::size_t value,
    std::size_t alignment)
{
    return (value + alignment - 1)
        & ~(alignment - 1);
}

bool KernelHeap::expand(std::size_t required_size)
{
    const std::size_t total_size =
        sizeof(Block) + required_size;

    const std::size_t pages =
        (total_size + page_size - 1)
        / page_size;

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
    std::size_t consumed_size)
{
    constexpr std::size_t minimum_payload = 8;

    /*
     * A useful remainder must be large enough for:
     *
     * new Block header
     * +
     * at least a tiny allocation
     */
    if (block->size <
        consumed_size
        + sizeof(Block)
        + minimum_payload)
    {
        return;
    }

    auto* new_block =
        reinterpret_cast<Block*>(
            reinterpret_cast<std::uintptr_t>(block)
            + sizeof(Block)
            + consumed_size);

    new_block->size =
        block->size
        - consumed_size
        - sizeof(Block);

    new_block->free = true;
    new_block->next = block->next;

    block->size = consumed_size;
    block->next = new_block;
}

void* KernelHeap::allocate(std::size_t size)
{
    return allocate(
        size,
        alignof(std::max_align_t));
}

void* KernelHeap::allocate(
    std::size_t size,
    std::size_t alignment)
{
    if (size == 0)
    {
        return nullptr;
    }

    /*
     * Alignment must be a nonzero power of two.
     */
    if (alignment == 0 ||
        (alignment & (alignment - 1)) != 0)
    {
        return nullptr;
    }

    for (;;)
    {
        Block* current = head_;

        while (current != nullptr)
        {
            if (!current->free)
            {
                current = current->next;
                continue;
            }

            const std::uintptr_t block_start =
                reinterpret_cast<std::uintptr_t>(
                    current);

            const std::uintptr_t payload_start =
                block_start + sizeof(Block);

            /*
             * Reserve one pointer immediately before
             * the returned address. free() uses this
             * to recover the owning Block.
             */
            const std::uintptr_t raw =
                payload_start + sizeof(Block*);

            const std::uintptr_t aligned =
                align_up(raw, alignment);

            /*
             * Total number of bytes consumed from
             * the block payload area.
             */
            std::size_t consumed =
                static_cast<std::size_t>(
                    aligned - payload_start)
                + size;

            /*
             * The next Block header itself must also
             * have suitable alignment.
             */
            consumed =
                align_up(
                    consumed,
                    alignof(Block));

            if (current->size >= consumed)
            {
                split(current, consumed);

                current->free = false;

                auto** owner_slot =
                    reinterpret_cast<Block**>(
                        aligned - sizeof(Block*));

                *owner_slot = current;

                return reinterpret_cast<void*>(
                    aligned);
            }

            current = current->next;
        }

        /*
         * Worst-case space required:
         *
         * owner pointer
         * + alignment padding
         * + requested object
         */
        const std::size_t required =
            sizeof(Block*)
            + (alignment - 1)
            + size;

        if (!expand(required))
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

    /*
     * The allocation stored its owning Block*
     * immediately before the returned pointer.
     */
    auto** owner_slot =
        reinterpret_cast<Block**>(
            reinterpret_cast<std::uintptr_t>(pointer)
            - sizeof(Block*));

    Block* block = *owner_slot;

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
                sizeof(Block)
                + next->size;

            current->next = next->next;

            continue;
        }

        current = current->next;
    }
}

}
