#include "ExtendedHv.h"

// Imports
// None

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_ExitBootServices(VOID);

// Private Globals
static EFI_EXIT_BOOT_SERVICES gOriginal;

// Private Functions
static EFI_STATUS EFIAPI HookedExitBootServices(IN EFI_HANDLE imageHandle, IN UINTN mapKey);



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
  // TODO
  // Add final logging and cleanup. Once this is called, winload.efi
  // has finished doing everything important and continued to transfer
  // into the ntoskrnl.
  // 


  //
  // Not our target, pass through to original
  //
  return gOriginal(imageHandle, mapKey);
}
