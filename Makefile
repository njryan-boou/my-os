BUILD_DIR := build
BOOT_SRC := src/boot/boot.asm
OS_IMAGE := $(BUILD_DIR)/os.bin

.PHONY: all run clean

all: $(OS_IMAGE)

$(OS_IMAGE): $(BOOT_SRC)
	mkdir -p $(BUILD_DIR)
	nasm -f bin $(BOOT_SRC) -o $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE)

clean:
	rm -rf $(BUILD_DIR)