#include <stdint.h>

typedef uint64_t (__attribute__((ms_abi)) *original_vmexit_handler_t)(void *arg1, void *arg2, void *context);

// Imports
// None

// Public Globals
__attribute__((section(".data.global")))
int64_t G_original_offset_from_hook = 0x0; // its value is set through a patch on the bytes and we have to ensure it comes first

// Public Functions
__attribute__((section(".text.function")))
uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(void *arg1, void *arg2, void *context); // this function is called through file offset and must come straight after the global

// Private Globals
// None

// Private Functions
// None


// Implementation

uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(void *arg1, void *arg2, void *context) {
  //
  // TODO
  // 
  __asm__ __volatile__ ( "int3" );

  //
  // Return original exit handler
  // 
  original_vmexit_handler_t original_handler = (original_vmexit_handler_t)((uint64_t)hooked_vmexit_handler + G_original_offset_from_hook);
  return original_handler(arg1, arg2, context);
}




