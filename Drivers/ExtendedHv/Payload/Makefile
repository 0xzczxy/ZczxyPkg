CC := gcc
LD := ld
OBJCOPY := objcopy

# Compiler flags
CFLAGS := -fPIC \
          -fno-asynchronous-unwind-tables \
          -fno-exceptions \
          -fno-stack-protector \
          -fno-stack-check \
          -mno-red-zone \
          -nostdlib \
          -nostartfiles \
          -nodefaultlibs \
          -Wall \
          -Wextra \
          -O2

# Output files
TARGET_ELF := payload.elf
TARGET_BIN := payload.bin

# Source files
SOURCES := src/main.c src/serial.c
OBJECTS := $(SOURCES:.c=.o)

# Linker script
LINKER_SCRIPT := payload.ld

# Default target
all: $(TARGET_BIN)

# Compile C sources to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into ELF using custom linker script
$(TARGET_ELF): $(OBJECTS) $(LINKER_SCRIPT)
	$(LD) -T $(LINKER_SCRIPT) $(OBJECTS) -o $@

# Extract raw binary from ELF
$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@
	@echo "✓ Generated $(TARGET_BIN)"
	@ls -lh $(TARGET_BIN)
	@echo "✓ Offsets:"
	@nm $(TARGET_ELF) | grep -E "G_original_offset_from_hook|hooked_vmexit_handler" || echo "  (Check with: nm $(TARGET_ELF))"

clean:
	rm -f $(OBJECTS) $(TARGET_ELF) $(TARGET_BIN)

.PHONY: all clean


