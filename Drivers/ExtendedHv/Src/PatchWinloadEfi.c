#include "Compiler.h"
#include "ExtendedHv.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern EFI_STATUS InstallPatch(patchinfo_t *info, void *originalFunction, void *targetFunction);

// Public Globals
BOOLEAN gBlLdrLoadImageReached = FALSE;
__attribute__((section(".text"))) patchinfo_t gBlLdrLoadImagePatchInfo = {0};

// Public Functions
EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction);

// Private Globals
// None

// Private Functions
static EFI_STATUS PatchedBlLdrLoadImage(
  VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
  VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
  VOID* arg14,  VOID* arg15, VOID* arg16, VOID* arg17
);

// Implementation

EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction) {
  //
  // Install Patch using patching utilities
  // 
  return InstallPatch(&gBlLdrLoadImagePatchInfo, originalFunction, PatchedBlLdrLoadImage);
}

static EFI_STATUS PatchedBlLdrLoadImage(
  VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
  VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
  VOID* arg14,  VOID* arg15, VOID* arg16, VOID* arg17
) {
  SerialPrint("Hook Entered.\n");
  SerialPrintHex("Trampoline Address", (UINT64)gBlLdrLoadImagePatchInfo.trampoline);
  SerialPrint("Size copied: %u bytes.\n", gBlLdrLoadImagePatchInfo.size);

  //
  // Call the original function
  // 
  const EFI_STATUS status =
    (
      (EFI_STATUS(*)(VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*))
      gBlLdrLoadImagePatchInfo.trampoline
    )
    (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17)
  ;

  SerialPrint("Trampoline Returned.\n");

  //
  // Check the function completed properly
  // 
  if (EFI_ERROR(status)) return status;

  SerialPrint("Completed successfully.\n");

  //
  // Set global so that exit boot services can know we got here
  // 
  gBlLdrLoadImageReached = TRUE;

  SerialPrint("Wrote to BlLdrLoadImageReached.\n");

  return status;
}

