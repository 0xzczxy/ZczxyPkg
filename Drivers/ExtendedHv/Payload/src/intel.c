#include "intel.h"
#include <stdint.h>
#include <stddef.h>

typedef uint64_t (__attribute__((ms_abi)) *original_vmexit_handler_t)(void *arg1, uint32_t exit_reason, uint32_t exit_reason_full);

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
uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(void *arg1, uint32_t exit_reason, uint32_t exit_reason_full);

// Private Globals
// None

// Private Functions
// None


// Implementation

uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(
    void *arg1, 
    uint32_t exit_reason, 
    uint32_t exit_reason_full
    /* int512_t arg3, */
    /* int512_t arg4, */
    // We don't consider these since it hasn't effected us yet
    // TODO: Don't program like this in such sensitive systems,
    //       feeling like a windows developer in this mindset
) {
  //
  // CPUID Exit Reason
  // 
  if (exit_reason == 0x0A) {
    //
    // Get pointer to guest register array
    //
    uint64_t *guest_regs = *(uint64_t**)arg1;

    //
    // Read guest RAX (first register, index 0)
    //
    uint64_t leaf_value = guest_regs[0];
    
    if (leaf_value == 0xDEADBEEFDEADBEEF) {
      //
      // Write to guest RAX
      // "zczxyhc\0" little endian hex
      //
      guest_regs[0] = 0x00636879787a637a;
    }

    //
    // TODO: We should be early returning but I still have no idea
    //       what this function returns.
    // 
  }
  
  //
  // Call original handler
  //
  original_vmexit_handler_t original = (original_vmexit_handler_t)(
    (uint64_t)hooked_vmexit_handler + G_original_offset_from_hook
  );
  return original(arg1, exit_reason, exit_reason_full /*, arg4, arg5 */);
}

