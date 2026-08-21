#include "Paging.hpp"

#include <cstddef>
#include <cstdint>

namespace kernel::memory {

namespace {

constexpr std::uintptr_t pdpt_address = 0x11000;
constexpr std::uintptr_t new_pd_address = 0x13000;
constexpr std::uintptr_t new_pt_address = 0x14000;

constexpr std::uint64_t present = 1ULL << 0;
constexpr std::uint64_t writable = 1ULL << 1;

volatile std::uint64_t* table(std::uintptr_t address)
{
    return reinterpret_cast<volatile std::uint64_t*>(address);
}

}

void Paging::map(
    std::uint64_t virtual_address,
    std::uint64_t physical_address)
{
    auto* pdpt = table(pdpt_address);
    auto* pd = table(new_pd_address);
    auto* pt = table(new_pt_address);

    /*
     * Virtual addresses beginning at 1 GiB use PDPT entry 1.
     *
     * PDPT[1] -> new page directory.
     */
    pdpt[1] =
        static_cast<std::uint64_t>(new_pd_address)
        | present
        | writable;

    /*
     * PD[0] -> page table.
     *
     * Unlike our boot-time mappings, this is NOT a 2 MiB huge page.
     */
    pd[0] =
        static_cast<std::uint64_t>(new_pt_address)
        | present
        | writable;

    /*
     * For the first 2 MiB beginning at virtual 0x40000000,
     * bits 12-20 select one of 512 4 KiB PT entries.
     */
    const std::size_t pt_index =
        static_cast<std::size_t>(
            (virtual_address >> 12) & 0x1FF);

    pt[pt_index] =
        physical_address
        | present
        | writable;

    /*
     * Invalidate this virtual address in the TLB.
     */
    asm volatile(
        "invlpg (%0)"
        :
        : "r"(virtual_address)
        : "memory");
}

}