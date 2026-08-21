BUILD_DIR := build

STAGE1_SRC := src/boot/stage1.asm
STAGE2_SRC := src/boot/stage2.asm

STAGE1_BIN := $(BUILD_DIR)/stage1.bin
STAGE2_BIN := $(BUILD_DIR)/stage2.bin
OS_IMAGE   := $(BUILD_DIR)/os.img

.PHONY: all run clean

all: $(OS_IMAGE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(STAGE1_BIN): $(STAGE1_SRC) | $(BUILD_DIR)
	nasm -f bin $(STAGE1_SRC) -o $(STAGE1_BIN)

$(STAGE2_BIN): $(STAGE2_SRC) | $(BUILD_DIR)
	nasm -f bin $(STAGE2_SRC) -o $(STAGE2_BIN)

$(OS_IMAGE): $(STAGE1_BIN) $(STAGE2_BIN)
	cat $(STAGE1_BIN) $(STAGE2_BIN) > $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE)

clean:
	rm -rf $(BUILD_DIR)