
# Makefile for user mode 'init' program

DEPDIR = .d
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

INIT_UNSTRIPPED = $(BUILD_DIR)/ui

SOURCES=$(wildcard *.c)
ASM_SOURCES=$(wildcard *.asm)
OBJECTS=$(ASM_SOURCES:%.asm=$(BUILD_DIR)/%.o) $(SOURCES:%.c=$(BUILD_DIR)/%.o)

$(shell mkdir -p $(DEPDIR) >/dev/null)

$(INIT_TARGET): $(OBJECTS)
	ld -m elf_i386 -Ttext=0x08000000 -o $(INIT_UNSTRIPPED) $(OBJECTS)
	objcopy -O binary -j .text -j .rodata -j .data $(INIT_UNSTRIPPED) $@

$(BUILD_DIR)/%.o : %.asm
	nasm -f elf32 $(OUTPUT_OPTION) $<

$(BUILD_DIR)/%.o : %.c
$(BUILD_DIR)/%.o : %.c $(DEPDIR)/%.d
	 $(COMPILE.c) $(OUTPUT_OPTION) $<
	 $(POSTCOMPILE)

clean:
	rm -rf $(DEPDIR)

.PHONY: clean

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCES)))
