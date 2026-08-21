#include "interrupts/IDT.hpp"
#include "memory/KernelHeap.hpp"
#include "memory/NewDelete.hpp"
#include "memory/PhysicalAllocator.hpp"
#include "terminal/Terminal.hpp"

#include <cstdint>

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    kernel::interrupts::initialize();

    kernel::memory::PhysicalAllocator allocator;
    kernel::memory::KernelHeap heap(allocator);

    kernel::memory::initialize_new_delete(heap);

    void* a = heap.allocate(64);
    void* b = heap.allocate(64);
    void* c = heap.allocate(64);

    terminal.write("A: ");
    terminal.write_hex(reinterpret_cast<std::uintptr_t>(a));
    terminal.write("\n");

    terminal.write("B: ");
    terminal.write_hex(reinterpret_cast<std::uintptr_t>(b));
    terminal.write("\n");

    terminal.write("C: ");
    terminal.write_hex(reinterpret_cast<std::uintptr_t>(c));
    terminal.write("\n");

    heap.free(b);
    heap.free(a);

    void* d = heap.allocate(128);

    terminal.write("D: ");
    terminal.write_hex(reinterpret_cast<std::uintptr_t>(d));
    terminal.write("\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}