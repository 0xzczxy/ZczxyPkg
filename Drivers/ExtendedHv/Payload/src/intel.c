#include "intel.h"
#include <stdint.h>
#include <stddef.h>

typedef uint64_t (__attribute__((ms_abi)) *original_vmexit_handler_t)(void *context_raw, uint32_t exit_reason, uint32_t exit_reason_full);

// Import
extern void serial_write(const char *string);
extern void serial_write_pointer(const char *msg, void *addr);
extern void serial_write_hex32(const char *msg, uint32_t value);
extern void serial_write_hex64(const char *msg, uint64_t value);
extern void serial_write_hex_dump(const char *msg, void *addr, uint32_t length);

// Public Globals
__attribute__((section(".data.global")))
int64_t G_original_offset_from_hook = 0x0;

// Public Functions
__attribute__((section(".text.function")))
uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(void *context_raw, uint32_t exit_reason, uint32_t exit_reason_full);

// Private Globals
static int g_vmexit_count = 0;
static const char g_hex_chars[] = "0123456789ABCDEF";

// Private Functions
static void analyze_context_for_cpuid(void *context_raw, uint32_t exit_reason);

// Implementation

uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(void *context_raw, uint32_t exit_reason, uint32_t exit_reason_full) {
  g_vmexit_count++;

  //
  // Special analysis for first CPUID exit
  //
  if (g_vmexit_count <= 5 && exit_reason == 0x0A) {
    analyze_context_for_cpuid(context_raw, exit_reason);
  }

  //
  // Call original handler
  //
  original_vmexit_handler_t original = (original_vmexit_handler_t)(
      (uint64_t)hooked_vmexit_handler + G_original_offset_from_hook
  );

  return original(context_raw, exit_reason, exit_reason_full);
}

static void analyze_context_for_cpuid(void *context_raw, uint32_t exit_reason) {
    uint64_t *qwords = (uint64_t *)context_raw;
    
    serial_write("\n========================================\n");
    serial_write("[+] CPUID Exit - Structure Analysis\n");
    serial_write("========================================\n");
    
    serial_write_hex32("\n[*] Exit reason", exit_reason);
    serial_write_pointer("[*] Context pointer", context_raw);
    
    serial_write("\n[*] Searching for CPUID leaf in first 256 bytes...\n");
    serial_write("    (CPUID leaf should be 0-0x20 for basic leaves)\n\n");
    
    // Dump first 32 qwords (256 bytes) and look for small values that could be CPUID leaves
    for (int i = 0; i < 32; i++) {
        uint64_t value = qwords[i];
        char offset_msg[32];
        
        // Format offset message
        offset_msg[0] = ' ';
        offset_msg[1] = ' ';
        offset_msg[2] = '+';
        offset_msg[3] = '0';
        offset_msg[4] = 'x';
        // Convert offset to hex (3 digits should be enough)
        int offset = i * 8;
        offset_msg[5] = g_hex_chars[(offset >> 8) & 0xF];
        offset_msg[6] = g_hex_chars[(offset >> 4) & 0xF];
        offset_msg[7] = g_hex_chars[offset & 0xF];
        offset_msg[8] = '\0';
        
        serial_write_hex64(offset_msg, value);
        
        // Check if this could be a CPUID leaf
        if (value < 0x100 && value != 0) {
            serial_write("      ^^^ POSSIBLE CPUID LEAF! ^^^\n");
        }
    }
    
    serial_write("\n[*] Full hex dump of first 512 bytes:\n");
    serial_write_hex_dump(NULL, context_raw, 512);
    
    serial_write("\n========================================\n\n");
}

