#include "ExtendedHv.h"

// Imports
// None

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_StartImage();

// Private Globals
static EFI_IMAGE_START gOriginal;

// Private Functions
static EFI_STATUS EFIAPI HookedStartImage(
  IN EFI_HANDLE,
  OUT UINTN *,
  OUT CHAR16 **_ OPTIONAL
);



EFI_STATUS InstallHook_StartImage() {
  //
  // Save original function
  //
  gOriginal = gBS->StartImage;
  if (!gOriginal) {
    SerialPrint("[!] Failed to save original function pointers\n");
    return EFI_UNSUPPORTED;
  }

  //
  // Install hook
  //
  gBS->StartImage = HookedStartImage;
  if (gBS->StartImage != HookedStartImage) {
    SerialPrint("[!] Failed to install hooks\n");
    return EFI_UNSUPPORTED;
  }

  SerialPrint("[+] StartImage Hook Installed: 0x%016lx -> 0x%016lx\n", (UINT64)gOriginal, (UINT64)HookedStartImage);

  return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI HookedStartImage(
  IN EFI_HANDLE imageHandle,
  OUT UINTN *exitDataSize,
  OUT CHAR16 **exitData OPTIONAL
) {
  EFI_STATUS status;
  
  SerialPrint("\n========== StartImage Called ==========\n");
  SerialPrintHex("ImageHandle", (UINT64)imageHandle);
  
  
  //
  // Call original StartImage
  //
  status = gOriginal(imageHandle, exitDataSize, exitData);
  
  SerialPrint("StartImage Status: %r\n", status);
  
  if (!EFI_ERROR(status)) {
    if (exitDataSize && *exitDataSize > 0) {
      SerialPrintHex("ExitDataSize", (UINT64)*exitDataSize);
      if (exitData && *exitData) {
        SerialPrintUnicode("ExitData", *exitData);
      }
    }
  }
  
  SerialPrint("=======================================\n\n");
  
  return status;
}


