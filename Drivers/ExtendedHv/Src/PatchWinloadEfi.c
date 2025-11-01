#include "ExtendedHv.h"

// Imports
extern CHAR16 *StriStr(IN CONST CHAR16 *string, IN CONST CHAR16 *searchString);
extern VOID *FindPattern(IN VOID *base, IN UINT64 size, IN UINT8 *pattern, IN UINT8 *mask, IN UINTN patternLen);
extern VOID *FindPatternPeExecutableSections(
  IN VOID *imageBase, IN UINT64 imageSize,
  IN VOID *pattern, IN VOID *mask,
  IN UINT64 pattern_size
);
extern EFI_STATUS InstallPatch(IN VOID *targetFunction, IN VOID *patchFunction, OUT UINT8 *originalBytes);


// Public Globals
// None

// Public Functions
EFI_STATUS PatchWinload(IN VOID *imageBase, IN UINT64 imageSize);

// Private Globals
static UINT8 gOriginalBytes[16] = {0};
static UINT64 gOriginalApplication = 0;
static UINT8 gPattern[] = {
	0x48, 0x8b, 0xc4, 0x48, 0x89, 0x58, 0x08, 0x48, 0x89, 0x70, 0x10, 0x48, 0x89, 0x78, 0x18, 0x4c,
	0x89, 0x70, 0x20, 0x55, 0x48, 0x8d, 0x68, 0xf1, 0x48, 0x81, 0xec, 0xa0, 0x00, 0x00, 0x00, 0x33,
	0xc0, 0x48, 0x8b, 0xf9, 0x8b, 0x0d, 0xde, 0x6f, 0x19, 0x00, 0x49, 0x8b, 0xf1, 0x89, 0x45, 0xe8,
	0x45, 0x8b, 0xf0, 0x66, 0x89, 0x45, 0xec, 0x48, 0x8b, 0xda, 0x88, 0x45, 0xee, 0x85, 0xc9, 0x74,
	0x26, 0x39, 0x05, 0xbd, 0x6f, 0x19, 0x00, 0x75, 0x1e, 0x8b, 0x05, 0xb1, 0x6f, 0x19, 0x00, 0xff,
	0xc0, 0x89, 0x05, 0xa9, 0x6f, 0x19, 0x00, 0x3b, 0xc1, 0x72, 0x0c, 0xe8, 0xc8, 0xb3, 0x03, 0x00,
	0x83, 0x25, 0x99, 0x6f, 0x19, 0x00, 0x00
};


// Private Functions
static UINT64 PatchedApplication(IN VOID *imageEntry, IN VOID *imageBase, IN UINT32 imageSize, IN VOID *returnArgs);



EFI_STATUS PatchWinload(IN VOID *imageBase, IN UINT64 imageSize) {
  EFI_STATUS status;
  VOID *app;
  
  SerialPrint(
    "\n"
    "========================================\n"
    "  Patching Winload.efi\n"
    "========================================\n"
  );
  SerialPrintHex("ImageBase", (UINT64)imageBase);
  SerialPrintHex("ImageSize", imageSize);

  //
  // Check Parameters
  // 
  if (!imageBase || imageSize == 0) {
    SerialPrint("[!] Invalid parameters\n");
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // Find BlImgLoadPEImageFromSourceBuffer
  //
  SerialPrint("[*] Searching for BlImgLoadPEImageFromSourceBuffer...\n");
  app = FindPatternPeExecutableSections(
    imageBase, imageSize,
    gPattern, NULL,
    sizeof(gPattern)
  );
  if (!app) {
    SerialPrint("[!] Failed to find BlImgLoadPEImageFromSourceBuffer\n");
    SerialPrint("[!] Winload patching failed\n");
    SerialPrint("========================================\n\n");
    return EFI_NOT_FOUND;
  }
  
  SerialPrintHex("BlImgLoadPEImageFromSourceBuffer", (UINT64)app);
  
  //
  // Install patch
  //
  SerialPrint("[*] Installing patch on BlImgLoadPEImageFromSourceBuffer...\n");
  status = InstallPatch(
    app,
    (VOID *)PatchedApplication,
    gOriginalBytes
  );
  
  if (EFI_ERROR(status)) {
    SerialPrint("[!] Failed to install patch: %r\n", status);
    SerialPrint("[!] Winload patching failed\n");
    SerialPrint("========================================\n\n");
    return status;
  }
  
  //
  // Save original function address
  //
  gOriginalApplication = (UINT64)app;
  
  SerialPrint("[+] Patch installed successfully\n");
  SerialPrintHex("Original bytes saved", (UINT64)gOriginalBytes);
  SerialPrint("[+] Bootmgfw.efi patched successfully\n");
  SerialPrint("[*] Will intercept calls to BlImgLoadPEImageFromSourceBuffer\n");
  SerialPrint("========================================\n\n");
  
  return EFI_SUCCESS;
}

static UINT64 PatchedApplication(
  IN VOID *imageEntry,
  IN VOID *imageBase,
  IN UINT32 imageSize,
  IN VOID *returnArgs
) {
  UINT64 result;
  
  SerialPrint(
    "\n"
    "************************************************\n"
    "  BlImgLoadPEImageFromSourceBuffer Called\n"
    "************************************************\n"
  );
  SerialPrintHex("ImageEntry", (UINT64)imageEntry);
  SerialPrintHex("ImageBase", (UINT64)imageBase);
  SerialPrintHex("ImageSize", (UINT64)imageSize);
  SerialPrintHex("ReturnArgs", (UINT64)returnArgs);
  SerialPrint("[*] Calling original BlImgLoadPEImageFromSourceBuffer...\n");
  SerialPrint("************************************************\n\n");

  //
  // Restore original bytes temporarily
  //
  CopyMem((VOID *)gOriginalApplication, gOriginalBytes, 5);
  
  //
  // Call original
  //
  typedef UINT64 (*BlImgStartBootApp_t)(VOID *, VOID *, UINT32, VOID *);
  BlImgStartBootApp_t originalFunc = (BlImgStartBootApp_t)gOriginalApplication;
  result = originalFunc(imageEntry, imageBase, imageSize, returnArgs);
  
  //
  // Re-install patch for potential future calls
  //
  UINT8 *target = (UINT8 *)gOriginalApplication;
  INT32 relativeOffset = (INT32)((UINT64)PatchedApplication - (UINT64)target - 5);
  target[0] = 0xE9;
  CopyMem(&target[1], &relativeOffset, sizeof(relativeOffset));
  
  return result;
}
