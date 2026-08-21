BUILD_DIR := build


# -------------------------
# Sources
# -------------------------

STAGE1_SRC := src/boot/stage1.asm
STAGE2_SRC := src/boot/stage2.asm

KERNEL_ENTRY_SRC       := src/kernel/entry.asm
KERNEL_SRC             := src/kernel/kernel.cpp
TERMINAL_SRC           := src/kernel/terminal/Terminal.cpp
IDT_SRC                := src/kernel/interrupts/IDT.cpp
INTERRUPTS_SRC         := src/kernel/interrupts/interrupts.asm
MEMORY_MAP_SRC         := src/kernel/memory/MemoryMap.cpp
PHYSICAL_ALLOCATOR_SRC := src/kernel/memory/PhysicalAllocator.cpp
PAGING_SRC             := src/kernel/memory/Paging.cpp
KERNEL_HEAP_SRC        := src/kernel/memory/KernelHeap.cpp
NEW_DELETE_SRC         := src/kernel/memory/NewDelete.cpp


# -------------------------
# Build outputs
# -------------------------

STAGE1_BIN := $(BUILD_DIR)/stage1.bin
STAGE2_BIN := $(BUILD_DIR)/stage2.bin

KERNEL_ENTRY_OBJ       := $(BUILD_DIR)/entry.o
KERNEL_OBJ             := $(BUILD_DIR)/kernel.o
TERMINAL_OBJ           := $(BUILD_DIR)/Terminal.o
IDT_OBJ                := $(BUILD_DIR)/IDT.o
INTERRUPTS_OBJ         := $(BUILD_DIR)/interrupts.o
MEMORY_MAP_OBJ         := $(BUILD_DIR)/MemoryMap.o
PHYSICAL_ALLOCATOR_OBJ := $(BUILD_DIR)/PhysicalAllocator.o
PAGING_OBJ             := $(BUILD_DIR)/Paging.o
KERNEL_HEAP_OBJ        := $(BUILD_DIR)/KernelHeap.o
NEW_DELETE_OBJ         := $(BUILD_DIR)/NewDelete.o

KERNEL_BIN := $(BUILD_DIR)/kernel.bin
KERNEL_PAD := $(BUILD_DIR)/kernel.pad

OS_IMAGE := $(BUILD_DIR)/os.img


# -------------------------
# Toolchain
# -------------------------

CXX := g++
LD  := ld

CXXFLAGS := \
	-m64 \
	-ffreestanding \
	-fno-exceptions \
	-fno-rtti \
	-fno-stack-protector \
	-fno-pie \
	-fno-pic \
	-fno-asynchronous-unwind-tables \
	-fno-unwind-tables \
	-mno-red-zone


# -------------------------
# Targets
# -------------------------

.PHONY: all run clean

all: $(OS_IMAGE)


# -------------------------
# Build directory
# -------------------------

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


# -------------------------
# Bootloader
# -------------------------

$(STAGE1_BIN): $(STAGE1_SRC) | $(BUILD_DIR)
	nasm -f bin $(STAGE1_SRC) -o $(STAGE1_BIN)

$(STAGE2_BIN): $(STAGE2_SRC) | $(BUILD_DIR)
	nasm -f bin $(STAGE2_SRC) -o $(STAGE2_BIN)


# -------------------------
# Kernel entry
# -------------------------

$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_SRC) | $(BUILD_DIR)
	nasm -f elf64 $(KERNEL_ENTRY_SRC) -o $(KERNEL_ENTRY_OBJ)


# -------------------------
# Kernel C++
# -------------------------

$(KERNEL_OBJ): $(KERNEL_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(KERNEL_SRC) -o $(KERNEL_OBJ)

$(TERMINAL_OBJ): $(TERMINAL_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(TERMINAL_SRC) -o $(TERMINAL_OBJ)

$(IDT_OBJ): $(IDT_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(IDT_SRC) -o $(IDT_OBJ)

$(MEMORY_MAP_OBJ): $(MEMORY_MAP_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(MEMORY_MAP_SRC) -o $(MEMORY_MAP_OBJ)

$(PHYSICAL_ALLOCATOR_OBJ): $(PHYSICAL_ALLOCATOR_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(PHYSICAL_ALLOCATOR_SRC) -o $(PHYSICAL_ALLOCATOR_OBJ)

$(PAGING_OBJ): $(PAGING_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(PAGING_SRC) -o $(PAGING_OBJ)

$(KERNEL_HEAP_OBJ): $(KERNEL_HEAP_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(KERNEL_HEAP_SRC) -o $(KERNEL_HEAP_OBJ)

$(NEW_DELETE_OBJ): $(NEW_DELETE_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(NEW_DELETE_SRC) -o $(NEW_DELETE_OBJ)


# -------------------------
# Interrupt assembly
# -------------------------

$(INTERRUPTS_OBJ): $(INTERRUPTS_SRC) | $(BUILD_DIR)
	nasm -f elf64 $(INTERRUPTS_SRC) -o $(INTERRUPTS_OBJ)


# -------------------------
# Link kernel
# -------------------------

$(KERNEL_BIN): \
	$(KERNEL_ENTRY_OBJ) \
	$(KERNEL_OBJ) \
	$(TERMINAL_OBJ) \
	$(IDT_OBJ) \
	$(INTERRUPTS_OBJ) \
	$(MEMORY_MAP_OBJ) \
	$(PHYSICAL_ALLOCATOR_OBJ) \
	$(PAGING_OBJ) \
	$(KERNEL_HEAP_OBJ) \
	$(NEW_DELETE_OBJ) \
	linker.ld
	$(LD) -T linker.ld \
		$(KERNEL_ENTRY_OBJ) \
		$(KERNEL_OBJ) \
		$(TERMINAL_OBJ) \
		$(IDT_OBJ) \
		$(INTERRUPTS_OBJ) \
		$(MEMORY_MAP_OBJ) \
		$(PHYSICAL_ALLOCATOR_OBJ) \
		$(PAGING_OBJ) \
		$(KERNEL_HEAP_OBJ) \
		$(NEW_DELETE_OBJ) \
		-Map=$(BUILD_DIR)/kernel.map \
		-o $(KERNEL_BIN)


# -------------------------
# Pad kernel
# -------------------------

$(KERNEL_PAD): $(KERNEL_BIN)
	test $$(stat -c%s $(KERNEL_BIN)) -le 8192
	cp $(KERNEL_BIN) $(KERNEL_PAD)
	truncate -s 8192 $(KERNEL_PAD)


# -------------------------
# Build OS image
# -------------------------

$(OS_IMAGE): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_PAD)
	cat \
		$(STAGE1_BIN) \
		$(STAGE2_BIN) \
		$(KERNEL_PAD) \
		> $(OS_IMAGE)


# -------------------------
# Run
# -------------------------

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE)


# -------------------------
# Clean
# -------------------------

clean:
	rm -rf $(BUILD_DIR)

osrun:
	./run.sh