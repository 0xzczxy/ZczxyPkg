#include "ExtendedHv.h"

// Imports
extern BOOLEAN IsBootmgfwEfi(IN CHAR16 *devicePath);
extern EFI_STATUS PatchBootmgfw(IN VOID *imageBase, IN UINT64 imageSize);

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_StartImage(VOID);

// Private Globals
static EFI_IMAGE_START gOriginal;

// Private Functions
static EFI_STATUS EFIAPI HookedStartImage(
  IN EFI_HANDLE imageHandle,
  OUT UINTN *exitDataSize,
  OUT CHAR16 **exitData OPTIONAL
);



EFI_STATUS InstallHook_StartImage(VOID) {
  //
  // Save original function
  //
  gOriginal = gBS->StartImage;
  if (!gOriginal) {
    SerialPrint("[!] Failed to save original function pointer\n");
    return EFI_UNSUPPORTED;
  }
  
  //
  // Install hook
  //
  gBS->StartImage = HookedStartImage;
  if (gBS->StartImage != HookedStartImage) {
    SerialPrint("[!] Failed to install hook\n");
    return EFI_UNSUPPORTED;
  }
  
  SerialPrint("[+] StartImage hook installed\n");
  SerialPrintHex("Original", (UINT64)gOriginal);
  SerialPrintHex("Hooked", (UINT64)HookedStartImage);
  
  return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI HookedStartImage(
  IN EFI_HANDLE imageHandle,
  OUT UINTN *exitDataSize,
  OUT CHAR16 **exitData OPTIONAL
) {
  EFI_STATUS status;
  EFI_LOADED_IMAGE_PROTOCOL *loadedImage;
  CHAR16 *pathText;
  
  SerialPrint(
    "\n"
    "========================================\n"
    "  StartImage Called  \n"
    "========================================\n"
  );
  SerialPrintHex("ImageHandle", (UINT64)imageHandle);
  
  //
  // Get loaded image protocol
  //
  status = gBS->HandleProtocol(
    imageHandle,
    &gEfiLoadedImageProtocolGuid,
    (VOID **)&loadedImage
  );
  if (EFI_ERROR(status)) {
    SerialPrint("[!] Failed to get loaded image protocol: %r\n", status);
    goto _passthrough;
  }

  //
  // Try to get file path
  //
  if (!loadedImage->FilePath) {
    SerialPrint("[!]Failed to get loadedImage->FilePath (corruption?)\n");
    goto _passthrough;
  }
  pathText = ConvertDevicePathToText(loadedImage->FilePath, TRUE, TRUE);
  if (!pathText) {
    SerialPrint("[!]Failed to convert loadedImage->FilePath to text\n");
    goto _passthrough;
  }
  
  //
  // Check if this is bootmgfw.efi
  //
  if (IsBootmgfwEfi(pathText)) {
    SerialPrint("[+] Detected bootmgfw.efi\n");
    
    //
    // Patch bootmgfw before execution
    //
    status = PatchBootmgfw(loadedImage->ImageBase, loadedImage->ImageSize);
    if (EFI_ERROR(status)) {
      SerialPrint("[!] Failed to patch bootmgfw: %r\n", status);
      goto _passthrough;
    }
    
    //
    // Execute patched image
    //
    status = gOriginal(imageHandle, exitDataSize, exitData);
    SerialPrint("========================================\n\n");
    return status;
  }

  FreePool(pathText);

_passthrough:
  SerialPrint("========================================\n\n");
  //
  // Not our target, pass through to original
  //
  return gOriginal(imageHandle, exitDataSize, exitData);
}
