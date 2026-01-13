#include "Compiler.h"
#include "ExtendedHv.h"
#include "WinDefines.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern EFI_STATUS InstallPatch(volatile patchinfo_t *info, void *originalFunction, void *targetFunction);
extern EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize);

// Public Globals
BOOLEAN gHvFound = FALSE;
patchinfo_t gBlLdrLoadImagePatchInfo;

// Public Functions
EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction);

// Private Globals
// None

// Private Functions
static EFI_STATUS EFIAPI PatchedBlLdrLoadImage(
  VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
  VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
  VOID* arg14,  VOID* arg15, VOID* arg16, VOID* arg17
);

// Implementation

EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction) {
  EFI_STATUS status;
  
  SerialPrint("[*] Installing BlLdrLoadImage patch\n");
  SerialPrintHex("  Target", (UINT64)originalFunction);
  
  status = InstallPatch(&gBlLdrLoadImagePatchInfo, originalFunction, PatchedBlLdrLoadImage);
  
  if (EFI_ERROR(status)) {
    SerialPrint("[!] InstallPatch failed: %r\n", status);
  } else {
    SerialPrint("[+] BlLdrLoadImage patch installed\n");
  }
  
  return status;
}

static EFI_STATUS EFIAPI PatchedBlLdrLoadImage(
  VOID* arg1, VOID* arg2, VOID *arg3, VOID *arg4, VOID* arg5, VOID* arg6, VOID* arg7,
  VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
  VOID* arg14,  VOID* arg15, VOID* arg16, VOID* arg17
) {
  //
  // Call the original function via trampoline
  // 
  const EFI_STATUS status =
    (
      (EFI_STATUS (EFIAPI *)(VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*))
      gBlLdrLoadImagePatchInfo.trampoline
    )
    (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17)
  ;

  //
  // Check the function completed properly
  // 
  if (EFI_ERROR(status)) return status;

  //
  // Check if we have already patched the hypervisor
  // 
  if (gHvFound) {
    return status;
  }

  //
  // Get arguments
  // 
  CHAR16* imagePath = (CHAR16*)arg3;
  CHAR16* imageName = (CHAR16*)arg4;
  const PLDR_DATA_TABLE_ENTRY entry = *(PPLDR_DATA_TABLE_ENTRY)arg9;

  //
  // Validate pointers
  //
  if (!imagePath || !imageName || !entry) {
    return status;
  }

  //
  // Compare image name to filter for hypervisor
  // 
  if (StrCmp(imageName, L"hv.exe") != 0) {
    return status;
  }

  //
  // Set flag that we reached this point
  // 
  gHvFound = TRUE;

  //
  // Patch Vm Exit Handler
  //
  InstallPatch_VmExitHandler(entry->ModuleBase, entry->SizeOfImage);

  return status;
}

