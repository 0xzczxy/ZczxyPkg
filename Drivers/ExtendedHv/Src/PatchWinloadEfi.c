#include "Compiler.h"
#include "ExtendedHv.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern EFI_STATUS InstallPatch(volatile patchinfo_t *info, void *originalFunction, void *targetFunction);

// Public Globals
BOOLEAN gBlLdrLoadImageReached = FALSE;

// Public Functions
EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction);

// Private Globals
__attribute__((section(".text"))) static volatile patchinfo_t gBlLdrLoadImagePatchInfo;

// Private Functions
static EFI_STATUS PatchedBlLdrLoadImage(
  VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
  VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
  VOID* arg14,  VOID* arg15, VOID* arg16, VOID* arg17
);

// Implementation

EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction) {
  EFI_STATUS status;
  
  //
  // Install Patch using patching utilities
  //
  status = InstallPatch(&gBlLdrLoadImagePatchInfo, originalFunction, PatchedBlLdrLoadImage);
  
  if (EFI_ERROR(status)) {
    SerialPrint("[!] InstallPatch failed: %r\n", status);
  }

  // SerialPrint("[DEBUG] Dumping trampoline buffer:\n");
  // unsigned char *buf = (unsigned char *)gBlLdrLoadImagePatchInfo.buffer;
  // SerialPrint("[DEBUG] First 32 bytes of buffer:\n");
  // for (int i = 0; i < 32; i++) {
  //     if (i % 16 == 0) SerialPrint("[DEBUG]   ");
  //     SerialPrint("%02x ", buf[i]);
  //     if ((i + 1) % 16 == 0) SerialPrint("\n");
  // }

  // SerialPrintHex("[DEBUG] Trampoline pointer", (UINT64)gBlLdrLoadImagePatchInfo.trampoline);
  // SerialPrintHex("[DEBUG] Original function", (UINT64)gBlLdrLoadImagePatchInfo.original_function);
  // SerialPrintHex("[DEBUG] Size", gBlLdrLoadImagePatchInfo.size);
  
  return status;
}

static EFI_STATUS PatchedBlLdrLoadImage(
  VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
  VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
  VOID* arg14,  VOID* arg15, VOID* arg16, VOID* arg17
) {
  SerialPrint("[+] Hook Entered.\n");
  SerialPrint("[+] gBlLdrLoadImagePatchInfo: 0x%p\n", &gBlLdrLoadImagePatchInfo);
  SerialPrint("[+] Trampoline Address: 0x%p\n", gBlLdrLoadImagePatchInfo.trampoline);
  SerialPrint("[+] Size copied: %u bytes.\n", gBlLdrLoadImagePatchInfo.size);

  // unsigned char *buf = (unsigned char *)gBlLdrLoadImagePatchInfo.trampoline;
  // SerialPrint("[DEBUG] About to execute from 0x%p\n", buf);
  // SerialPrint("[DEBUG] First 16 bytes: ");
  // for (int i = 0; i < 16; i++) {
  //     SerialPrint("%02x ", buf[i]);
  // }
  // SerialPrint("\n");

  //
  // Call the original function via trampoline
  // 
  const EFI_STATUS status =
    (
      (EFI_STATUS(*)(VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*))
      gBlLdrLoadImagePatchInfo.trampoline
    )
    (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17)
  ;

  SerialPrint("[+] Trampoline Returned.\n");

  //
  // Check the function completed properly
  // 
  if (EFI_ERROR(status)) return status;

  SerialPrint("[+] Completed successfully.\n");

  //
  // Set global so that exit boot services can know we got here
  // 
  gBlLdrLoadImageReached = TRUE;

  SerialPrint("[+] Wrote to BlLdrLoadImageReached.\n");

  return status;
}
