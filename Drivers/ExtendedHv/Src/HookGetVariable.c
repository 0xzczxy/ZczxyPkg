#include "Compiler.h"
#include "ExtendedHv.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern UINT64 PeGetExport(IN CONST VOID *base, IN CONST CHAR8 *export);
extern EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction);
extern EFI_STATUS InstallPatch_BlImgAllocateImageBuffer(IN VOID *originalFunction);

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_GetVariable(VOID);

// Private Globals
static EFI_GET_VARIABLE gOriginal;
static BOOLEAN gPatchesInstalled = FALSE;

// Private Functions
static EFI_STATUS EFIAPI HookedGetVariable(
  CHAR16* variableName, EFI_GUID* vendorGuid,
  UINT32* attributes,
  UINTN* dataSize, VOID* data
);

// Implementation

EFI_STATUS InstallHook_GetVariable(VOID) {
  //
  // Save original function
  //
  gOriginal = gRT->GetVariable;
  if (!gOriginal) {
    SerialPrint("[!] Failed to save original function pointer\n");
    return EFI_UNSUPPORTED;
  }
  
  //
  // Install hook
  //
  gRT->GetVariable = HookedGetVariable;
  if (gRT->GetVariable != HookedGetVariable) {
    SerialPrint("[!] Failed to install hook\n");
    return EFI_UNSUPPORTED;
  }
  
  SerialPrint("[+] GetVariable hook installed\n");
  SerialPrintHex("Original", (UINT64)gOriginal);
  SerialPrintHex("Hooked", (UINT64)HookedGetVariable);
  
  return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI HookedGetVariable(
  CHAR16* variableName, EFI_GUID* vendorGuid,
  UINT32* attributes,
  UINTN* dataSize, VOID* data
) {
  UINT64 returnAddress = 0;
  UINT64 moduleBase = 0;
  UINT64 blLdrLoadImageAddr = 0;
  UINT64 blImgAllocateImageBufferAddr = 0;
  
  //
  // Only process SetupMode variable
  //
  if (StrCmp(variableName, L"SetupMode")) {
    goto _pass;
  }

  //
  // Only install patches once
  //
  if (gPatchesInstalled) {
    goto _pass;
  }

  SerialPrint("\n");
  SerialPrint("================================================\n");
  SerialPrint("  Winload.efi Detection\n");
  SerialPrint("================================================\n");

  //
  // Scan backwards from return address to find PE header
  // Look for "This program cannot be run in DOS mode" string
  //
  returnAddress = (UINT64)GetReturnAddress();
  while (CompareMem((VOID*)returnAddress, "This program cannot be run in DOS mode", 38) != 0) {
    returnAddress--;
  }

  //
  // DOS stub is at offset 0x4E from base
  // 
  moduleBase = returnAddress - 0x4E;

  SerialPrint("[+] Found potential winload.efi\n");
  SerialPrintHex("Module Base", moduleBase);

  //
  // Find BlImgAllocateImageBuffer export
  // 
  blImgAllocateImageBufferAddr = PeGetExport((VOID*)moduleBase, "BlImgAllocateImageBuffer");
  if (!blImgAllocateImageBufferAddr) {
    SerialPrint("[!] Failed to find BlImgAllocateImageBuffer export\n");
    goto _pass;
  }
  
  SerialPrint("[+] Found export 'BlImgAllocateImageBuffer'\n");
  SerialPrintHex("Address", blImgAllocateImageBufferAddr);

  //
  // Find BlLdrLoadImage export
  // 
  blLdrLoadImageAddr = PeGetExport((VOID*)moduleBase, "BlLdrLoadImage");
  if (!blLdrLoadImageAddr) {
    SerialPrint("[!] Failed to find BlLdrLoadImage export\n");
    goto _pass;
  }
  
  SerialPrint("[+] Found export 'BlLdrLoadImage'\n");
  SerialPrintHex("Address", blLdrLoadImageAddr);

  //
  // Install BlImgAllocateImageBuffer patch
  // This extends the memory allocation to fit our custom section in hv.exe
  //
  SerialPrint("\n[*] Installing allocation hook...\n");
  if (EFI_ERROR(InstallPatch_BlImgAllocateImageBuffer((VOID*)blImgAllocateImageBufferAddr))) {
    SerialPrint("[!] Failed to patch BlImgAllocateImageBuffer\n");
    goto _pass;
  }

  //
  // Install BlLdrLoadImage patch SECOND
  // This injects our payload when hv.exe is loaded
  //
  SerialPrint("\n[*] Installing load image hook...\n");
  if (EFI_ERROR(InstallPatch_BlLdrLoadImage((VOID*)blLdrLoadImageAddr))) {
    SerialPrint("[!] Failed to patch BlLdrLoadImage\n");
    goto _pass;
  }

  SerialPrint("\n[+] All patches installed successfully\n");
  SerialPrint("================================================\n\n");
  
  //
  // Mark patches as installed
  //
  gPatchesInstalled = TRUE;
    
  //
  // Remove our hook
  // 
  gRT->GetVariable = gOriginal; 

_pass:
  //
  // Pass through to original
  //
  return gOriginal(variableName, vendorGuid, attributes, dataSize, data);
}
