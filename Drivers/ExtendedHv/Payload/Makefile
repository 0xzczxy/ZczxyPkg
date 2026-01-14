CC := gcc
LD := ld
OBJCOPY := objcopy

# Compiler flags
CFLAGS := --save-temps -fPIC -fno-plt -fvisibility=hidden \
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

# Build directory
BUILD := build

# Linker script
LINKER_SCRIPT := payload.ld

# Payload definitions
INTEL_SRC := src/intel.c src/serial.c
AMD_SRC   := src/amd.c  src/serial.c

INTEL_OBJS := $(INTEL_SRC:%.c=$(BUILD)/%.o)
AMD_OBJS   := $(AMD_SRC:%.c=$(BUILD)/%.o)

INTEL_ELF := $(BUILD)/payload-intel.elf
INTEL_BIN := $(BUILD)/payload-intel.bin

AMD_ELF := $(BUILD)/payload-amd.elf
AMD_BIN := $(BUILD)/payload-amd.bin

# Default target
all: $(INTEL_BIN) $(AMD_BIN)

# Ensure build directory exists
$(BUILD):
	mkdir -p $(BUILD)/src

# Compile rule
$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Link rules
$(INTEL_ELF): $(INTEL_OBJS) $(LINKER_SCRIPT)
	$(LD) -T $(LINKER_SCRIPT) $(INTEL_OBJS) -o $@

$(AMD_ELF): $(AMD_OBJS) $(LINKER_SCRIPT)
	$(LD) -T $(LINKER_SCRIPT) $(AMD_OBJS) -o $@

# Binary extraction
$(INTEL_BIN): $(INTEL_ELF)
	$(OBJCOPY) -O binary $< $@
	@echo "✓ Generated $(INTEL_BIN)"
	@ls -lh $(INTEL_BIN)
	@echo "✓ Offsets:"
	@nm $(INTEL_ELF) | grep -E "G_original_offset_from_hook|G_arch|hooked_vmexit_handler" || echo "  (Check with: nm $(INTEL_ELF))"

$(AMD_BIN): $(AMD_ELF)
	$(OBJCOPY) -O binary $< $@
	@echo "✓ Generated $(AMD_BIN)"
	@ls -lh $(AMD_BIN)
	@echo "✓ Offsets:"
	@nm $(AMD_ELF) | grep -E "G_original_offset_from_hook|G_arch|hooked_vmexit_handler" || echo "  (Check with: nm $(AMD_ELF))"

clean:
	rm -rf $(BUILD)

.PHONY: all clean
