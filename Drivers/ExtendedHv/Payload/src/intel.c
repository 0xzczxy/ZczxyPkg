#include <stdint.h>

// Public Globals
__attribute__((section(".data.global")))
int64_t G_original_offset_from_hook = 0x0; // its value is set through a patch on the bytes and we have to ensure it comes first

// Public Functions
__attribute__((section(".text.function")))
uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(/* TODO */); // this function is called through file offset and must come straight after the global

// Private Globals
// None

// Private Functions
// None


// Implementation


uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(/* TODO */) {




  return 0;  
}

