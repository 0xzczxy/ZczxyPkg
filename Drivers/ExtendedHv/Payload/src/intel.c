#include <stdint.h>
#include <stddef.h>

//
// Reference Intel Manual
// 
#define VMCS_EXIT_REASON                    0x00004402
#define VMCS_GUEST_RIP                      0x0000681E
#define VMCS_GUEST_RSP                      0x0000681C
#define VMCS_VMEXIT_INSTRUCTION_LENGTH      0x0000440C
#define VMX_EXIT_REASON_EXECUTE_CPUID       0x0000000A

struct context {
  uint64_t rax;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rbx;
  uint64_t rsp;
  uint64_t rbp;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
};
typedef struct context context_t;

typedef uint64_t (__attribute__((ms_abi)) *original_vmexit_handler_t)(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4);

// Import
extern void serial_write(const char *string);
extern void serial_write_hex64(const char *msg, uint64_t value);

// Public Globals
__attribute__((section(".data.global")))
int64_t G_original_offset_from_hook = 0x0;

// Public Functions
__attribute__((section(".text.function")))
uint64_t __attribute__((ms_abi)) vmexit_handler(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4);

// Private Globals
static int g_has_been_called = 0;

// Private Functions
static inline uint64_t vmread(uint64_t field);
static inline void vmwrite(uint64_t field, uint64_t value);


// Implementation

//
// Reference: https://github.com/noahware/hyper-reV/blob/main/hyperv-attachment/src/main.cpp
// 
uint64_t __attribute__((ms_abi)) vmexit_handler(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
  if (!g_has_been_called) {
    g_has_been_called = 1;
    serial_write("[+] Intel VM-Exit Handler Initialized.\n");
  }

  //
  // Read through vmread(VMCS_EXIT_REASON)
  // 
  uint64_t exit_reason = vmread(VMCS_EXIT_REASON);

  //
  // Check if we this was caused by a cpuid
  // 
  if (exit_reason == VMX_EXIT_REASON_EXECUTE_CPUID) {
    context_t *ctx = *(context_t**)a1;

    //
    // Check if rax is set to our special value
    // 
    if (ctx->rax == 0xDEADBEEFDEADBEEFull) {
      serial_write("[*] CPUID Called with 0xDEADBEEFDEADBEEF!\n");

      //
      // Update guest RSP in VMCS
      // 
      ctx->rsp = vmread(VMCS_GUEST_RSP);

      //
      // custom return value: zczxyhc\0 in little endian
      // 
      ctx->rax = 0x00636879787A637Aull;

      //
      // Advance guest RIP
      // 
      uint64_t guest_rip = vmread(VMCS_GUEST_RIP);
      uint64_t instruction_length = vmread(VMCS_VMEXIT_INSTRUCTION_LENGTH);

      serial_write_hex64("[+] Rip Before", guest_rip);
      serial_write_hex64("[+] Instruction Length", instruction_length);

      uint64_t next_rip = guest_rip + instruction_length;
      vmwrite(VMCS_GUEST_RIP, next_rip);

      serial_write_hex64("[+] Rip After", vmread(VMCS_GUEST_RIP));

      //
      // Write back RSP to VMCS
      // 
      vmwrite(VMCS_GUEST_RSP, ctx->rsp);

      return 0;
    } else if (ctx->rax == 0x00636879787A637Aull) {
      serial_write("[!] CPUID Called with value supposed to be returned!\n");
    }
  }

  //
  // Call original handler for other cases
  //
  original_vmexit_handler_t original = (original_vmexit_handler_t)(
    (uint64_t)vmexit_handler + G_original_offset_from_hook
  );
  return original(a1, a2, a3, a4);
}


static inline uint64_t vmread(uint64_t field) {
  uint64_t value;
  __asm__ volatile("vmread %1, %0" : "=r"(value) : "r"(field) : "cc");
  return value;
}
   
static inline void vmwrite(uint64_t field, uint64_t value) {
  __asm__ volatile("vmwrite %1, %0" : : "r"(field), "r"(value) : "cc");
}

