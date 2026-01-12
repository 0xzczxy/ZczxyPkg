#include "Compiler.h"
#include "ExtendedHv.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern EFI_STATUS InstallPatch(OUT patchinfo_t *info, IN VOID *originalFunction, IN VOID *targetFunction);
extern EFI_STATUS UninstallPatch(IN OUT patchinfo_t *info);

// Public Globals
BOOLEAN gBlLdrLoadImageReached = FALSE;
patchinfo_t gBlLdrLoadImagePatchInfo = {0};

// Public Functions
EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction);

// Private Globals
// None

// Private Functions
static EFI_STATUS PatchedBlLdrLoadImage(
  VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
  VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
  VOID* arg14, VOID* arg15, VOID* arg16, VOID* arg17
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
  VOID* arg14, VOID* arg15, VOID* arg16, VOID* arg17
) {
  EFI_STATUS status;
  
  SerialPrint("Hook Entered.\n");
  
  //
  // Temporarily uninstall the patch to call the original function
  //
  UninstallPatch(&gBlLdrLoadImagePatchInfo);
  SerialPrint("Patch temporarily removed.\n");
  
  //
  // Call the original function (now restored)
  //
  status =
    (
      (EFI_STATUS(*)(VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*))
      gBlLdrLoadImagePatchInfo.original_function
    )
    (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17)
  ;
  SerialPrint("Original function returned.\n");
  
  //
  // Check the function completed properly
  //
  if (EFI_ERROR(status)) goto _exit;
  SerialPrint("Completed successfully.\n");

  //
  // Set global so that exit boot services can know we got here
  //
  gBlLdrLoadImageReached = TRUE;
  SerialPrint("Wrote to BlLdrLoadImageReached.\n");

  return status;

_exit:

  //
  // Reinstall the patch for next call
  //
  InstallPatch(
    &gBlLdrLoadImagePatchInfo, 
    gBlLdrLoadImagePatchInfo.original_function, 
    PatchedBlLdrLoadImage
  );
  SerialPrint("Patch reinstalled.\n");

  return status;
}
