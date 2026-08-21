#include "NewDelete.hpp"

#include "KernelHeap.hpp"

#include <cstddef>

namespace {

kernel::memory::KernelHeap* kernel_heap = nullptr;

[[noreturn]]
void allocation_failure()
{
    for (;;)
    {
        asm volatile("cli; hlt");
    }
}

}

namespace kernel::memory {

void initialize_new_delete(KernelHeap& heap)
{
    kernel_heap = &heap;
}

}

void* operator new(std::size_t size)
{
    if (kernel_heap == nullptr)
    {
        allocation_failure();
    }

    void* pointer = kernel_heap->allocate(size);

    if (pointer == nullptr)
    {
        allocation_failure();
    }

    return pointer;
}

void operator delete(void* pointer) noexcept
{
    if (kernel_heap != nullptr)
    {
        kernel_heap->free(pointer);
    }
}

void operator delete(
    void* pointer,
    std::size_t) noexcept
{
    operator delete(pointer);
}