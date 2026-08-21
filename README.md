# my-os

A small x86-64 operating system built from scratch with C++ and NASM. It uses a
two-stage BIOS bootloader to enter long mode and launch a freestanding kernel.

Current features include VGA text output, an interrupt descriptor table, BIOS
E820 memory-map detection, and a physical page allocator.

## Requirements

- GNU Make
- NASM
- GCC and GNU binutils
- QEMU (`qemu-system-x86_64`)

## Build and run

```sh
make
make run
```

The bootable disk image is written to `build/os.img`.

To remove generated files:

```sh
make clean
```
