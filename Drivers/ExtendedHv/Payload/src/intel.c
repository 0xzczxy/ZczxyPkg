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
  // CPUID Exit Reason (0x0A = 10 decimal)
  // 
  if (exit_reason == 0x0A) {
    //
    // Get pointer to guest register array
    //
    uint64_t *guest_regs = *(uint64_t**)arg1;

    //
    // Read guest RAX (CPUID leaf - first register, index 0)
    //
    uint64_t leaf_value = guest_regs[0];
    
    if (leaf_value == 0xDEADBEEFDEADBEEF) {
      //
      // Set custom RAX value
      // "zczxyhc\0" little endian hex
      //
      guest_regs[0] = 0x00636879787a637a;
      
      //
      // Set VM state to 0x1f (advance RIP and resume)
      // This tells the hypervisor to skip the CPUID instruction
      //
      void **base = (void **)arg1;
      *(uint32_t*)&base[-0x1d8] = 0x1f;
      
      //
      // Return early without calling original handler
      // Return value is arg1[-0x160]
      //
      return (uint64_t)base[-0x160];
    }
  }
  
  //
  // Call original handler for other cases
  //
  original_vmexit_handler_t original = (original_vmexit_handler_t)(
    (uint64_t)hooked_vmexit_handler + G_original_offset_from_hook
  );
  return original(arg1, exit_reason, exit_reason_full);
}

