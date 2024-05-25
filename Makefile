PROJECT := linux_loader
TARGETS := bin/$(PROJECT).elf bin/$(PROJECT).bin

.PHONY: $(TARGETS)

all: $(TARGETS)

PREFIX := riscv64-unknown-elf-
CFLAGS := -fno-stack-protector
CFLAGS += -static-libgcc -fdata-sections -ffunction-sections -Wbuiltin-declaration-mismatch 
CFLAGS += -g -O3 -march=rv32ima -mabi=ilp32 -static
LDFLAGS:= -T linker.lds -nostdlib -Wl,--gc-sections

C_TARGETS := src/main.c src/utils/utils.c src/utils/printf.c src/fat/fat.c

bin/$(PROJECT).elf: $(C_TARGETS) src/entry.S
	@$(PREFIX)gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@$(PREFIX)strip -s -R .comment -R .gnu.version --strip-unneeded $@

bin/$(PROJECT).bin: bin/$(PROJECT).elf
	@$(PREFIX)objcopy $^ -O binary $@

test: bin/$(PROJECT).bin
	@cd ../mini-rv32ima/mini-rv32ima; make mini-rv32ima
	@sudo ../mini-rv32ima/mini-rv32ima/mini-rv32ima -f $< -z /dev/loop31

clean :
	@rm -rf $(TARGETS)