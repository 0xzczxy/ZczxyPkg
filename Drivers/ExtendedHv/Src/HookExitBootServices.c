#include "ExtendedHv.h"

// Imports
// None

// Public Globals
extern UINT64 gBlLdrLoadImageCallCount;
extern BOOLEAN gHvDetected;
extern UINT64 gHvImageBase;
extern UINTN gHvImageSize;

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
  // Report winload statistics
  // 
  SerialPrint("[*] BlLdrLoadImage was called %lu times\n", gBlLdrLoadImageCallCount);
  
  if (gBlLdrLoadImageCallCount == 0) {
    SerialPrint("[!] WARNING: Hook was never called!\n");
    SerialPrint("[!] Winload patch failed\n");
  } else {
    SerialPrint("[+] Winload patch working correctly\n");
  }

  //
  // Report hv.exe detection
  //
  if (gHvDetected) {
    SerialPrint("\n[+] HV.EXE DETECTED!\n");
    SerialPrintHex("  Base address", gHvImageBase);
    SerialPrintHex("  Image size", gHvImageSize);
    SerialPrint("[*] Ready for hypervisor patching\n");
  } else {
    SerialPrint("\n[!] HV.EXE NOT DETECTED\n");
    SerialPrint("[!] System not using Hyper-V or VBS disabled\n");
  }

  SerialPrint("========================================\n\n");

  //
  // Pass through to original
  //
  return gOriginal(imageHandle, mapKey);
}
