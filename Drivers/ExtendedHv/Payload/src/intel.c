#include "intel.h"

#include <stdint.h>

typedef uint64_t (__attribute__((ms_abi)) *original_vmexit_handler_t)(context_t *context, uint32_t exit_reason, uint32_t exit_reason_full);

// Import
extern void serial_write(const char *string);
extern void serial_write_pointer(const char *msg, void *addr);

// Public Globals
__attribute__((section(".data.global")))
int64_t G_original_offset_from_hook = 0x0; // its value is set through a patch on the bytes and we have to ensure it comes first

// Public Functions
__attribute__((section(".text.function")))
uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(context_t *context, uint32_t exit_reason, uint32_t exit_reason_full);

// Private Globals
static int g_vmexit_called = 0;

// Private Functions
// None

// Implementation

uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(context_t *context, uint32_t exit_reason, uint32_t exit_reason_full) {
  //
  // Attempt to debug serial print on the first run through (serial printing could fail at such a mature state in the os)
  //
  if (!g_vmexit_called) {
    g_vmexit_called = 1;
    serial_write("[+] Intel VM-Exit handler active\n");

    serial_write_pointer("[+] offset", (void*)hooked_vmexit_handler);
    serial_write_pointer("[+] offset", (void*)&G_original_offset_from_hook);

    serial_write_pointer("[+] offset", (void*)G_original_offset_from_hook);

    serial_write_pointer("[+] arg0", (void*)context);
  }


  //
  // Call original handler
  //
  original_vmexit_handler_t original = (original_vmexit_handler_t)(
      (uint64_t)hooked_vmexit_handler + G_original_offset_from_hook
  );

  return original(context, exit_reason, exit_reason_full);
}


