#include "ExtendedHv.h"

// Imports
extern BOOLEAN gBlLdrLoadImageReached;
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_ExitBootServices(VOID);

// Private Globals
static EFI_EXIT_BOOT_SERVICES gOriginal;

// Private Functions
static EFI_STATUS EFIAPI HookedExitBootServices(IN EFI_HANDLE imageHandle, IN UINTN mapKey);


// Implementation

EFI_STATUS InstallHook_ExitBootServices(VOID) {
  //
  // Save original function
  //
  gOriginal = gBS->ExitBootServices;
  if (!gOriginal) {
    SerialPrint("[!] Failed to save original function pointer\n");
    return EFI_UNSUPPORTED;
  }
  
  //
  // Install hook
  //
  gBS->ExitBootServices = HookedExitBootServices;
  if (gBS->ExitBootServices != HookedExitBootServices) {
    SerialPrint("[!] Failed to install hook\n");
    return EFI_UNSUPPORTED;
  }
  
  SerialPrint("[+] ExitBootServices hook installed\n");
  SerialPrintHex("Original", (UINT64)gOriginal);
  SerialPrintHex("Hooked", (UINT64)HookedExitBootServices);
  
  return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI HookedExitBootServices(IN EFI_HANDLE imageHandle, IN UINTN mapKey) {
  SerialPrint(
    "\n"
    "========================================\n"
    "  ExitBootServices Called  \n"
    "========================================\n"
  );

  //
  // Check if winload.efi was patched correctly
  // 
  if (gBlLdrLoadImageReached) {
    SerialPrint("gBlLdrLoadImageReached TRUE, patch working correctly.\n");
  } else {
    SerialPrint("gBlLdrLoadImageReached FALSE, patch not working.\n");
  }

  //
  // Pass through to original
  //
  return gOriginal(imageHandle, mapKey);
}
