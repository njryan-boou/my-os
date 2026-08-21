#include "Paging.hpp"

#include "PhysicalAllocator.hpp"

#include <cstddef>
#include <cstdint>

namespace kernel::memory {

namespace {

constexpr std::uintptr_t pdpt_address = 0x11000;

constexpr std::uint64_t present  = 1ULL << 0;
constexpr std::uint64_t writable = 1ULL << 1;

volatile std::uint64_t* table(std::uint64_t address)
{
    return reinterpret_cast<volatile std::uint64_t*>(address);
}

void clear_table(volatile std::uint64_t* table_ptr)
{
    for (std::size_t i = 0; i < 512; ++i)
    {
        table_ptr[i] = 0;
    }
}

}

bool Paging::map(
    std::uint64_t virtual_address,
    std::uint64_t physical_address,
    PhysicalAllocator& allocator)
{
    auto* pdpt = table(pdpt_address);

    const std::size_t pdpt_index =
        static_cast<std::size_t>(
            (virtual_address >> 30) & 0x1FF);

    const std::size_t pd_index =
        static_cast<std::size_t>(
            (virtual_address >> 21) & 0x1FF);

    const std::size_t pt_index =
        static_cast<std::size_t>(
            (virtual_address >> 12) & 0x1FF);

    std::uint64_t pd_address;

    if ((pdpt[pdpt_index] & present) == 0)
    {
        pd_address = allocator.allocate();

        if (pd_address == 0)
        {
            return false;
        }

        auto* pd = table(pd_address);
        clear_table(pd);

        pdpt[pdpt_index] =
            pd_address | present | writable;
    }
    else
    {
        pd_address =
            pdpt[pdpt_index] & 0x000FFFFFFFFFF000ULL;
    }

    auto* pd = table(pd_address);

    std::uint64_t pt_address;

    if ((pd[pd_index] & present) == 0)
    {
        pt_address = allocator.allocate();

        if (pt_address == 0)
        {
            return false;
        }

        auto* pt = table(pt_address);
        clear_table(pt);

        pd[pd_index] =
            pt_address | present | writable;
    }
    else
    {
        pt_address =
            pd[pd_index] & 0x000FFFFFFFFFF000ULL;
    }

    auto* pt = table(pt_address);

    pt[pt_index] =
        physical_address | present | writable;

    asm volatile(
        "invlpg (%0)"
        :
        : "r"(virtual_address)
        : "memory");

    return true;
}

}