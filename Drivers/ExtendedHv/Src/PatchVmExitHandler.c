#include "ExtendedHv.h"

typedef struct {
  UINT64 Low;
  UINT64 High;
} UINT128;

typedef struct __declspec(align(16)) GUEST_CONTEXT {
  UINT8 Reserved1[8];
  UINT64 Rcx;
  UINT64 Rdx;
  UINT64 Rbx;
  UINT8 Reserved2[8];
  UINT64 Rbp;
  UINT64 Rsi;
  UINT64 Rdi;
  UINT64 R8;
  UINT64 R9;
  UINT64 R10;
  UINT64 R11;
  UINT64 R12;
  UINT64 R13;
  UINT64 R14;
  UINT64 R15;
  UINT128 Xmm0;
  UINT128 Xmm1;
  UINT128 Xmm2;
  UINT128 Xmm3;
  UINT128 Xmm4;
  UINT128 Xmm5;
  UINT128 Xmm6;
  UINT128 Xmm7;
  UINT128 Xmm8;
  UINT128 Xmm9;
  UINT128 Xmm10;
  UINT128 Xmm11;
  UINT128 Xmm12;
  UINT128 Xmm13;
  UINT128 Xmm14;
  UINT128 Xmm15;
  UINT8 Reserved3[8];
  UINT64 VmcbPhysicalAddress;
} GUEST_CONTEXT, * PGUEST_CONTEXT;

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);

// Public Globals
// None

// Public Functions
EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize);
UINT64 PatchSizeVmExitHandler(VOID);

// Private Globals
static BOOLEAN gVmExitCalled = FALSE;

// Private Functions
static UINT64 PatchedVmExitHandler(VOID* arg1, VOID* arg2, PGUEST_CONTEXT context);
static VOID PatchedVmExitHandlerEnd(); // This function has to be allocated and this symbol is to know the size

// Implementation

UINT64 PatchSizeVmExitHandler(VOID) {
  return PatchedVmExitHandler - PatchedVmExitHandlerEnd;
}

EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize) {
  //
  // Add new section to store our patched function
  // 


  //
  // Pattern scan for the vmrun instruction as a byte sequence
  // 

  
  //
  // Install the patch
  // 


  return EFI_SUCCESS;
}

static UINT64 PatchedVmExitHandler(VOID* arg1, VOID* arg2, PGUEST_CONTEXT context) {
  if (!gVmExitCalled) {
    gVmExitCalled = TRUE;

    //
    // Debug Print
    // 
    SerialPrint("Reached into the VmExitHandler.\n");

    //
    // Initialize Memory
    // 
    

  }

  //
  // Check if VmExit was called by cpuid
  // 
  

  //
  // Check if VtlReturn was called (VTL1 to VTL0 Translation)
  // 


  //
  // Return original
  // 


}
static VOID PatchedVmExitHandlerEnd() { volatile int dont_optimize_please; (void)dont_optimize_please; } // This function has to be allocated and this symbol is to know the size

