#include "ExtendedHv.h"

// Imports
// None

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_ExitBootServices();

// Private Globals
static EFI_EXIT_BOOT_SERVICES gOriginal;

// Private Functions
static EFI_STATUS EFIAPI HookedExitBootServices(IN EFI_HANDLE, IN UINTN MapKey);



EFI_STATUS InstallHook_ExitBootServices() {
  SerialPrint("\n[*] Installing ExitBootServices hook...\n");
  
  //
  // Save original memory address of function
  //
  gOriginal = gBS->ExitBootServices;
  if (gOriginal != gBS->ExitBootServices) {
    SerialPrint("[!] Failed to assign gBS->ExitBootServices to gOriginal\n");
    return EFI_UNSUPPORTED;
  }
  
  //
  // Override memory address to our function
  //
  gBS->ExitBootServices = HookedExitBootServices;
  if (gBS->ExitBootServices != &HookedExitBootServices) {
    SerialPrint("[!] Failed to override gBS->ExitBootServices\n");
    return EFI_UNSUPPORTED;
  }
  
  SerialPrint("[+] ExitBootServices hook installed\n");
  SerialPrint("    Original: 0x%016lx\n", (UINT64)gOriginal);
  SerialPrint("    Hook:     0x%016lx\n", (UINT64)HookedExitBootServices);
  
  //
  // Return success
  //
  return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI HookedExitBootServices(IN EFI_HANDLE imageHandle, IN UINTN mapKey) {
  //
  // This is the last time we can use Boot Services
  // After this, only Runtime Services and serial port remain
  //
  SerialPrint("\n");
  SerialPrint("========================================\n");
  SerialPrint("  ExitBootServices Called!\n");
  SerialPrint("========================================\n");
  SerialPrintHex("ImageHandle", (UINT64)imageHandle);
  SerialPrintHex("MapKey", mapKey);
  SerialPrint("[*] Transitioning to OS loader...\n");
  SerialPrint("[*] Boot Services will no longer be available\n");
  SerialPrint("========================================\n\n");
  
  //
  // Return to original
  //
  return gOriginal(imageHandle, mapKey);
}

