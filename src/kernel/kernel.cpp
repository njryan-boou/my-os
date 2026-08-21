#include "interrupts/IDT.hpp"
#include "memory/KernelHeap.hpp"
#include "memory/NewDelete.hpp"
#include "memory/PhysicalAllocator.hpp"
#include "terminal/Terminal.hpp"

#include <cstdint>

namespace {

class TestObject
{
public:
    TestObject(
        std::uint64_t x,
        std::uint64_t y)
        : x_(x),
          y_(y)
    {
    }

    [[nodiscard]]
    std::uint64_t sum() const
    {
        return x_ + y_;
    }

private:
    std::uint64_t x_;
    std::uint64_t y_;
};

}

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    kernel::interrupts::initialize();

    kernel::memory::PhysicalAllocator physical_allocator;
    kernel::memory::KernelHeap heap(physical_allocator);

    kernel::memory::initialize_new_delete(heap);

    auto* object =
        new TestObject(10, 32);

    terminal.write("Object address: ");
    terminal.write_hex(
        reinterpret_cast<std::uint64_t>(object));
    terminal.write("\n");

    terminal.write("Object result:  ");
    terminal.write_hex(object->sum());
    terminal.write("\n");

    delete object;

    auto* second =
        new TestObject(100, 200);

    terminal.write("Second address: ");
    terminal.write_hex(
        reinterpret_cast<std::uint64_t>(second));
    terminal.write("\n");

    terminal.write("Second result:  ");
    terminal.write_hex(second->sum());
    terminal.write("\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}