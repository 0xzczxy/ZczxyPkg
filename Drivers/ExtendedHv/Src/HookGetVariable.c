#include "Compiler.h"
#include "ExtendedHv.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern UINT64 PeGetExport(IN CONST VOID *base, IN CONST CHAR8 *export);
extern EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction);

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_GetVariable(VOID);

// Private Globals
static EFI_GET_VARIABLE gOriginal;

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
  
  //
  // Setup mode variable is called within winload.efi
  //
  if (StrCmp(variableName, L"SetupMode")) goto _pass;

  //
  // Scan for common string inside a PE header (slow)
  //
  returnAddress = (UINT64)GetReturnAddress();
  while (CompareMem((VOID*)returnAddress, "This program cannot be run in DOS mode", 38) != 0) {
      returnAddress--;
  }
  moduleBase = returnAddress - 0x4E;

  SerialPrint("Found potential winload.efi\n");
  SerialPrintHex("Module Base", moduleBase);

  //
  // Find BlLdrLoadImage through export table (more reliable then pattern searching)
  // 
  blLdrLoadImageAddr = PeGetExport((VOID*)moduleBase, "BlLdrLoadImage");
  if (!blLdrLoadImageAddr) goto _pass;
  SerialPrint("Found export 'BlLdrLoadImage'\n");

  //
  // Patch Winload
  // 
  if (EFI_ERROR(InstallPatch_BlLdrLoadImage((VOID*)blLdrLoadImageAddr))) {
    SerialPrint("Failed to patch BlLdrLoadImage.\n");
    // Don't jump, we have done our hook work so we let it end.
  } else {
    SerialPrint("Patched BlLdrLoadImage successfully.\n");
  }
    
  //
  // We have completed our work, remove hook
  // 
  gRT->GetVariable = gOriginal; 

  //
  // Pass through to original
  //
_pass:
  return gOriginal(variableName, vendorGuid, attributes, dataSize, data);
}



