#include "intel.h"

#include <stdint.h>

typedef uint64_t (__attribute__((ms_abi)) *original_vmexit_handler_t)(context_t *context, uint32_t exit_reason, uint32_t exit_reason_full);

// Import
extern void serial_write(const char *string);
extern void serial_write_pointer(const char *msg, void *addr);
extern void serial_write_hex32(const char *msg, uint32_t value);
extern void serial_write_hex64(const char *msg, uint64_t value);
extern void serial_write_hex_dump(const char *msg, void *addr, uint32_t length);

// Public Globals
__attribute__((section(".data.global")))
int64_t G_original_offset_from_hook = 0x0; // its value is set through a patch on the bytes and we have to ensure it comes first

// Public Functions
__attribute__((section(".text.function")))
uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(context_t *context, uint32_t exit_reason, uint32_t exit_reason_full);

// Private Globals
static int g_vmexit_called = 0;

// Private Functions
static void dump_context_structure(context_t *context);


// Implementation

uint64_t __attribute__((ms_abi)) hooked_vmexit_handler(context_t *context, uint32_t exit_reason, uint32_t exit_reason_full) {
  //
  // Attempt to debug serial print on the first run through
  //
  if (!g_vmexit_called) {
    g_vmexit_called = 1;
    
    serial_write("\n========================================\n");
    serial_write("[+] Intel VM-Exit Handler Active\n");
    serial_write("========================================\n");

    //
    // Print function addresses and offsets
    // 
    serial_write("\n[*] Function Information:\n");
    serial_write_pointer("  hooked_vmexit_handler addr", (void*)hooked_vmexit_handler);
    serial_write_pointer("  G_original_offset addr", (void*)&G_original_offset_from_hook);
    serial_write_hex64("  G_original_offset value", (uint64_t)G_original_offset_from_hook);

    //
    // Print arguments
    // 
    serial_write("\n[*] Function Arguments:\n");
    serial_write_pointer("  arg0 (context ptr)", (void*)context);
    serial_write_hex32("  arg1 (exit_reason)", exit_reason);
    serial_write_hex32("  arg2 (exit_reason_full)", exit_reason_full);

    //
    // Decode exit reason
    // 
    serial_write("\n[*] Exit Reason Analysis:\n");
    serial_write("Common exit reasons:\n");
    serial_write("  0x00 = Exception/NMI\n");
    serial_write("  0x01 = External interrupt\n");
    serial_write("  0x0A = CPUID\n");
    serial_write("  0x0C = HLT\n");
    serial_write("  0x0E = INVLPG\n");
    serial_write("  0x10 = RDTSC\n");
    serial_write("  0x12 = VMCALL\n");
    serial_write("  0x1C = CR access\n");
    serial_write("  0x1E = I/O instruction\n");
    serial_write("  0x30 = WRMSR\n");

    //
    // Dump context structure
    // 
    serial_write("\n[*] Context Structure Dump:\n");
    dump_context_structure(context);

    //
    // Dump raw memory around context
    // 
    serial_write("\n[*] Raw Context Memory (first 512 bytes):\n");
    serial_write_hex_dump(NULL, context, 512);

    serial_write("\n========================================\n");
    serial_write("[+] Continuing to original handler...\n");
    serial_write("========================================\n\n");
  }

  //
  // Call original handler
  //
  original_vmexit_handler_t original = (original_vmexit_handler_t)(
    (uint64_t)hooked_vmexit_handler + G_original_offset_from_hook
  );

  return original(context, exit_reason, exit_reason_full);
}

static void dump_context_structure(context_t *context) {
  serial_write(" General Purpose Registers:\n");
  serial_write_hex64("  RAX", context->rax);
  serial_write_hex64("  RCX", context->rcx);
  serial_write_hex64("  RDX", context->rdx);
  serial_write_hex64("  RBX", context->rbx);
  serial_write_hex64("  RSP", context->rsp);
  serial_write_hex64("  RBP", context->rbp);
  serial_write_hex64("  RSI", context->rsi);
  serial_write_hex64("  RDI", context->rdi);
  serial_write_hex64("  R8 ", context->r8);
  serial_write_hex64("  R9 ", context->r9);
  serial_write_hex64("  R10", context->r10);
  serial_write_hex64("  R11", context->r11);
  serial_write_hex64("  R12", context->r12);
  serial_write_hex64("  R13", context->r13);
  serial_write_hex64("  R14", context->r14);
  serial_write_hex64("  R15", context->r15);

  serial_write("\n XMM Registers (showing upper/lower 64-bits):\n");
  serial_write_hex64("  XMM0 upper", context->xmm0._upper);
  serial_write_hex64("  XMM0 lower", context->xmm0._lower);
  serial_write_hex64("  XMM1 upper", context->xmm1._upper);
  serial_write_hex64("  XMM1 lower", context->xmm1._lower);
  serial_write_hex64("  XMM2 upper", context->xmm2._upper);
  serial_write_hex64("  XMM2 lower", context->xmm2._lower);

  // Sanity checks
  serial_write("\n Sanity Checks:\n");
  if (context->rsp > 0xFFFF800000000000ULL) {
    serial_write("  [OK] RSP is in kernel address space\n");
  } else {
    serial_write("  [!!] RSP is NOT in expected kernel range\n");
  }

  // Check if context pointer itself is in kernel space
  uint64_t ctx_addr = (uint64_t)context;
  if (ctx_addr > 0xFFFF800000000000ULL) {
    serial_write("  [OK] Context pointer is in kernel space\n");
  } else {
    serial_write("  [!!] Context pointer is NOT in kernel space\n");
  }
}
