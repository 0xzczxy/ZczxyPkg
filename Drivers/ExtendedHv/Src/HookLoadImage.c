#include "ExtendedHv.h"

// Imports
// None
extern EFI_STATUS InstallHook_OpenProtocol();

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_LoadImage();

// Private Globals
static EFI_IMAGE_LOAD gOriginal;

// Private Functions
static EFI_STATUS EFIAPI HookedLoadImage(
  IN BOOLEAN,
  IN EFI_HANDLE,
  IN EFI_DEVICE_PATH_PROTOCOL *,
  IN VOID *_ OPTIONAL,
  IN UINTN,
  OUT EFI_HANDLE *
);



EFI_STATUS InstallHook_LoadImage() {
  //
  // Save original function
  //
  gOriginal = gBS->LoadImage;
  if (!gOriginal) {
    SerialPrint("[!] Failed to save original function pointers\n");
    return EFI_UNSUPPORTED;
  }

  //
  // Install hook
  //
  gBS->LoadImage = HookedLoadImage;
  if (gBS->LoadImage != HookedLoadImage) {
    SerialPrint("[!] Failed to install hooks\n");
    return EFI_UNSUPPORTED;
  }

  SerialPrint("[+] LoadImage Hook Installed: 0x%016lx -> 0x%016lx\n", (UINT64)gOriginal, (UINT64)HookedLoadImage);

  return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI HookedLoadImage(
  IN BOOLEAN bootPolicy,
  IN EFI_HANDLE parentImageHandle,
  IN EFI_DEVICE_PATH_PROTOCOL *devicePath,
  IN VOID *sourceBuffer OPTIONAL,
  IN UINTN sourceSize,
  OUT EFI_HANDLE *imageHandle
) {
  EFI_STATUS status;

  //
  // Install Hook on OpenProtocol (PRIMARY HOOK FOR FINDING WINLOAD)
  //
  status = InstallHook_OpenProtocol();
  if (EFI_ERROR(status)) {
    SerialPrint("[!] Failed to install 'OpenProtocol' hook: %r\n", status);
    Print(L"[!] Failed to install 'OpenProtocol' hook: %r\n", status);
  }
  
  SerialPrint("\n========== LoadImage Called ==========\n");
  SerialPrint("BootPolicy:        %a\n", bootPolicy ? "TRUE" : "FALSE");
  SerialPrintHex("ParentImageHandle", (UINT64)parentImageHandle);
  SerialPrintHex("DevicePath", (UINT64)devicePath);
  SerialPrintHex("SourceBuffer", (UINT64)sourceBuffer);
  SerialPrintHex("SourceSize", (UINT64)sourceSize);

  //
  // Call original LoadImage
  //
  status = gOriginal(
    bootPolicy,
    parentImageHandle,
    devicePath,
    sourceBuffer,
    sourceSize,
    imageHandle
  );

  SerialPrint("LoadImage Status:  %r\n", status);

  if (!EFI_ERROR(status) && imageHandle) {
    SerialPrintHex("ImageHandle (out)", (UINT64)*imageHandle);
  }

  SerialPrint("======================================\n\n");

  return status;
}


