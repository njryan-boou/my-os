#include <memory/NewDelete.hpp>

#include <memory/KernelHeap.hpp>

#include <cstddef>
#include <new>

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

void* operator new[](std::size_t size)
{
    return operator new(size);
}

void operator delete[](void* pointer) noexcept
{
    operator delete(pointer);
}

void operator delete[](
    void* pointer,
    std::size_t) noexcept
{
    operator delete(pointer);
}

void* operator new(
    std::size_t size,
    std::align_val_t alignment)
{
    if (kernel_heap == nullptr)
    {
        allocation_failure();
    }

    void* pointer = kernel_heap->allocate(
        size,
        static_cast<std::size_t>(alignment));

    if (pointer == nullptr)
    {
        allocation_failure();
    }

    return pointer;
}

void* operator new[](
    std::size_t size,
    std::align_val_t alignment)
{
    return operator new(size, alignment);
}

void operator delete(
    void* pointer,
    std::align_val_t) noexcept
{
    operator delete(pointer);
}

void operator delete[](
    void* pointer,
    std::align_val_t) noexcept
{
    operator delete(pointer);
}

void operator delete(
    void* pointer,
    std::size_t,
    std::align_val_t) noexcept
{
    operator delete(pointer);
}

void operator delete[](
    void* pointer,
    std::size_t,
    std::align_val_t) noexcept
{
    operator delete(pointer);
}
