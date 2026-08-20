ASM := nasm
QEMU := qemu-system-x86_64

SRC := src/boot/boot.asm
BUILD := build
BIN := $(BUILD)/boot.bin

.PHONY: all run debug disasm hex clean

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p $(BUILD)
	$(ASM) -f bin $(SRC) -o $(BIN)

run: $(BIN)
	$(QEMU) -drive format=raw,file=$(BIN)

debug: $(BIN)
	$(QEMU) -drive format=raw,file=$(BIN) -s -S

disasm: $(BIN)
	ndisasm -b 16 -o 0x7c00 $(BIN)

hex: $(BIN)
	xxd -g 1 $(BIN)

clean:
	rm -rf $(BUILD)