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
    explicit TestObject(std::uint64_t value)
        : value_(value)
    {
    }

    [[nodiscard]]
    std::uint64_t value() const
    {
        return value_;
    }

private:
    std::uint64_t value_;
};

}

extern "C" void kernel_main()
{
    kernel::Terminal terminal;

    terminal.clear();
    kernel::interrupts::initialize();

    kernel::memory::PhysicalAllocator allocator;
    kernel::memory::KernelHeap heap(allocator);

    terminal.write("Heap ready.\n");

    kernel::memory::initialize_new_delete(heap);

    terminal.write("new/delete initialized.\n");

    auto* object = new TestObject(42);

    terminal.write("new returned.\n");

    terminal.write("Object address: ");
    terminal.write_hex(
        reinterpret_cast<std::uint64_t>(object));
    terminal.write("\n");

    terminal.write("Object value: ");
    terminal.write_hex(object->value());
    terminal.write("\n");

    delete object;

    terminal.write("delete returned.\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}