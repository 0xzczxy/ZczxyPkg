#include "payload.h"

typedef uint64_t (__attribute__((ms_abi)) *original_vmexit_handler_t)(void *arg1, void *arg2, void *context);

#define ARCH_UNKNOWN 0
#define ARCH_INTEL 1
#define ARCH_AMD 2

// Imports
extern void serial_write(const char *);

// Public Globals
__attribute__((section(".data.global")))
int64_t G_original_offset_from_hook = 0xAABBCCDDEEFF0011; // its value is set through a patch on the bytes and we have to ensure it comes first
__attribute__((section(".data.global")))
int32_t G_arch = 0x0; // its value is set through a patch on the bytes and we have to ensure it comes first

// Public Functions
__attribute__((section(".text.function")))
uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(void *arg1, void *arg2, void *context); // this function is called through file offset and must come straight after the global

// Private Globals
static int g_vmexit_called = 0;

// Private Functions
// None


// Implementation

uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(void *arg1, void *arg2, void *context) {
  //
  // REBOOT
  // 
  __asm__ __volatile__ (
    "outb %0, %1"
    :
    : "a"((unsigned char)0xFE), "Nd"((unsigned char)0x64)
    : "memory"
  );
  __asm__ __volatile__ (
    "outb %0, %1"
    :
    : "a"((unsigned char)0x06), "Nd"((unsigned short)0xCF9)
    : "memory"
  );
  
  //
  // CRASH SYSTEM
  // 
  *(volatile uint64_t*)0 = 0;

  //
  // Fall back, Infinite Loop
  // 
  while (1);

  //
  // First time initialization
  // 
  if (!g_vmexit_called) {
    g_vmexit_called = 1;
    serial_write("[+] Hello from the vmexit handler!");
  }

  //
  // Call original vmexit handler
  // 
  original_vmexit_handler_t original_handler = (original_vmexit_handler_t)((uint64_t)hooked_vmexit_handler + G_original_offset_from_hook);
  return original_handler(arg1, arg2, context);
}


