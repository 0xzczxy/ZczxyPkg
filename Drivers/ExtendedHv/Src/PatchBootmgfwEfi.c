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
extern EFI_STATUS PatchWinload(IN VOID *imageBase, IN UINT64 imageSize);

// Public Functions
BOOLEAN IsBootmgfwEfi(IN CHAR16 *devicePath);
EFI_STATUS PatchBootmgfw(IN VOID *imageBase, IN UINT64 imageSize);

// Private Globals
static UINT8 gOriginalBytes[16] = {0};
static VOID *gOriginalFunction = NULL;
static BOOLEAN gPatchCalled = FALSE;

// Pattern for ImgArchStartBootApplication
static UINT8 gPattern[] = {
	0x48, 0x8b, 0xc4, 0x48, 0x89, 0x58, 0x20, 0x44, 0x89, 0x40, 0x18, 0x48, 0x89, 0x50, 0x10, 0x48,
	0x89, 0x48, 0x08, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8d,
	0x68, 0xa9, 0x48, 0x81, 0xec, 0xc0, 0x00, 0x00, 0x00, 0x48, 0x8b, 0xf1, 0x45, 0x8b, 0xc0, 0x33,
	0xc9, 0x0f, 0x57, 0xc0, 0x33, 0xff, 0x66, 0x89, 0x4d, 0xf7, 0x41, 0x8b, 0xd9, 0x48, 0x89, 0x7d,
	0x0f, 0x4c, 0x8d, 0x4d, 0x07, 0x48, 0x89, 0x7d, 0x07, 0x0f, 0x11, 0x45, 0xd7, 0x0f, 0x11, 0x45,
	0xe7, 0xe8, 0xaa, 0xe5, 0xec, 0xff, 0x48, 0x8b, 0x45, 0x07, 0xba, 0x64, 0x86, 0x00, 0x00, 0x66,
	0x39, 0x50, 0x04, 0x74, 0x0a, 0xb8, 0xbb, 0x00, 0x00, 0xc0, 0xe9, 0x61, 0x01, 0x00, 0x00, 0x48,
	0x89, 0x7d, 0xbf, 0xb8, 0xff, 0xff, 0xff, 0xff, 0x48, 0x89, 0x7d, 0xaf, 0x41, 0xb5, 0x01, 0x89,
	0x45, 0xa3, 0x4c, 0x8b, 0xff, 0x44, 0x88, 0x6d, 0x9f, 0x4c, 0x8b, 0xf7, 0x8b, 0xf8, 0x0f, 0xba,
	0xe3, 0x0c, 0x72, 0x21, 0x48, 0x8d, 0x4e, 0x2c, 0xba, 0x60, 0x00, 0x00, 0x16, 0x4c, 0x8d, 0x45,
	0x9f, 0xe8, 0xf6, 0x9d, 0xfd, 0xff, 0x44, 0x0f, 0xb6, 0x6d, 0x9f, 0x33, 0xdb, 0x85, 0xc0, 0x44,
	0x0f, 0x48, 0xeb, 0xeb, 0x02, 0x33, 0xdb, 0x33, 0xd2, 0x4c, 0x8d, 0x45, 0xc7, 0x0f, 0x57, 0xc0,
	0x0f, 0x11, 0x45, 0xc7, 0x8d, 0x4a, 0x07, 0xe8, 0x3c, 0x81, 0x0c, 0x00, 0xf7, 0x45, 0xcf, 0x00,
	0x00, 0x01, 0x00, 0x0f, 0x84, 0x1e, 0x01, 0x00, 0x00, 0xe8, 0x46, 0xdc, 0x09, 0x00, 0x44, 0x8a,
	0xe0, 0x84, 0xc0, 0x75, 0x6a, 0x48, 0x8d, 0x4e, 0x2c, 0xba, 0x88, 0x00, 0x00, 0x15, 0x4c, 0x8d,
	0x45, 0x0f, 0xe8, 0xc5, 0x9e, 0xfd, 0xff, 0x85, 0xc0, 0x78, 0x54, 0x48, 0x83, 0x7d, 0x0f, 0x02
};

// Private Functions
static UINT64 EFIAPI PatchedArchStartBootApplication(
  IN INT64 *arg1, 
  IN VOID *imageBase,
  IN UINT32 imageSize, 
  IN INT32 arg4, 
  IN INT128_t *arg5
);

BOOLEAN IsBootmgfwEfi(IN CHAR16 *pathString) {
  BOOLEAN result;
  
  SerialPrint("[*] Checking path: %s\n", pathString);
  
  //
  // Check if path contains bootmgfw.efi
  //
  result = (StriStr(pathString, L"bootmgfw.efi") != NULL);
  
  return result;
}

EFI_STATUS PatchBootmgfw(IN VOID *imageBase, IN UINT64 imageSize) {
  EFI_STATUS status;
  VOID *archStartApp;
  
  if (!imageBase || imageSize == 0) {
    SerialPrint("[!] Invalid parameters\n");
    return EFI_INVALID_PARAMETER;
  }
  
  SerialPrint(
    "\n"
    "========================================\n"
    "  Patching Bootmgfw.efi\n"
    "========================================\n"
  );
  SerialPrintHex("ImageBase", (UINT64)imageBase);
  SerialPrintHex("ImageSize", imageSize);
  
  //
  // Find ImgArchStartBootApplication
  //
  SerialPrint("[*] Searching for ImgArchStartBootApplication...\n");
  archStartApp = FindPatternPeExecutableSections(
    imageBase, imageSize,
    gPattern, NULL,
    sizeof(gPattern)
  );
  if (!archStartApp) {
    SerialPrint("[!] Failed to find ImgArchStartBootApplication\n");
    SerialPrint("[!] Bootmgfw patching failed\n");
    SerialPrint("========================================\n\n");
    return EFI_NOT_FOUND;
  }
  
  SerialPrintHex("ImgArchStartBootApplication", (UINT64)archStartApp);
  
  //
  // Store original function pointer
  //
  gOriginalFunction = archStartApp;
  
  //
  // Install patch (one-time hook)
  //
  SerialPrint("[*] Installing one-time hook on ImgArchStartBootApplication...\n");
  status = InstallPatch(
    archStartApp,
    (VOID *)PatchedArchStartBootApplication,
    gOriginalBytes
  );
  
  if (EFI_ERROR(status)) {
    SerialPrint("[!] Failed to install patch: %r\n", status);
    SerialPrint("[!] Bootmgfw patching failed\n");
    SerialPrint("========================================\n\n");
    return status;
  }
  
  SerialPrint("[+] One-time hook installed successfully\n");
  SerialPrintHex("Original bytes saved", (UINT64)gOriginalBytes);
  SerialPrint("[+] Bootmgfw.efi patched successfully\n");
  SerialPrint("[*] Will intercept first call to ImgArchStartBootApplication\n");
  SerialPrint("========================================\n\n");
  
  return EFI_SUCCESS;
}

static UINT64 EFIAPI PatchedArchStartBootApplication(
  IN INT64 *arg1, 
  IN VOID *imageBase,
  IN UINT32 imageSize, 
  IN INT32 arg4, 
  IN INT128_t *arg5
) {
  UINT64 result;
  EFI_STATUS status;
  EFI_IMAGE_DOS_HEADER *dosHeader;
  EFI_IMAGE_NT_HEADERS64 *ntHeaders;
  typedef UINT64 (EFIAPI *ImgArchStart_t)(INT64*, VOID*, UINT32, INT32, INT128_t*);
  ImgArchStart_t originalFunc;
  
  SerialPrint(
    "\n"
    "************************************************\n"
    "  ImgArchStartBootApplication Called\n"
    "************************************************\n"
  );
  
  //
  // Check if this is the first call
  //
  if (gPatchCalled) {
    SerialPrint("[!] WARNING: Function called again after first interception!\n");
    SerialPrint("[*] This should not happen - calling original directly\n");
    originalFunc = (ImgArchStart_t)gOriginalFunction;
    return originalFunc(arg1, imageBase, imageSize, arg4, arg5);
  }
  
  gPatchCalled = TRUE;
  
  SerialPrintHex("[*] arg1 (boot options)", (UINT64)arg1);
  SerialPrintHex("[*] Image Base (arg2)", (UINT64)imageBase);
  SerialPrintHex("[*] Image Size (arg3)", imageSize);
  SerialPrintHex("[*] arg4 (flags)", arg4);
  SerialPrintHex("[*] arg5 (return args)", (UINT64)arg5);
  
  //
  // Validate PE image
  //
  dosHeader = (EFI_IMAGE_DOS_HEADER *)imageBase;
  SerialPrintHex("[*] DOS Magic", dosHeader->e_magic);
  
  if (dosHeader->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    SerialPrint("[+] Valid DOS signature (MZ)!\n");
    
    ntHeaders = (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)imageBase + dosHeader->e_lfanew);
    SerialPrintHex("[*] NT Signature", ntHeaders->Signature);
    
    if (ntHeaders->Signature == EFI_IMAGE_NT_SIGNATURE) {
      SerialPrint("[+] Valid PE signature!\n");
      SerialPrintHex("[+] Machine type", ntHeaders->FileHeader.Machine);
      SerialPrintHex("[+] SizeOfImage", ntHeaders->OptionalHeader.SizeOfImage);
      
      //
      // Verify this is winload.efi (x64 executable)
      //
      if (ntHeaders->FileHeader.Machine == 0x8664) {
        SerialPrint("[+] This is a valid x64 PE image (likely winload.efi)\n");
        
        //
        // Patch winload NOW!
        //
        SerialPrint("[*] Attempting to patch winload...\n");
        status = PatchWinload(imageBase, imageSize);
        if (EFI_ERROR(status)) {
          SerialPrint("[!] Failed to patch winload: %r\n", status);
        } else {
          SerialPrint("[+] *** WINLOAD PATCHED SUCCESSFULLY! ***\n");
        }
      } else {
        SerialPrint("[!] Not an x64 image, skipping patch\n");
      }
    } else {
      SerialPrintHex("[!] Invalid NT signature", ntHeaders->Signature);
    }
  } else {
    SerialPrintHex("[!] Invalid DOS signature", dosHeader->e_magic);
  }
  
  //
  // Restore original bytes BEFORE calling the function
  // This removes our hook and restores the original code
  //
  SerialPrint("[*] Restoring original bytes...\n");
  CopyMem(gOriginalFunction, gOriginalBytes, 16);
  SerialPrint("[+] Original function restored\n");
  
  //
  // Now call the original function (with original bytes restored)
  //
  SerialPrint("[*] Calling original ImgArchStartBootApplication...\n");
  originalFunc = (ImgArchStart_t)gOriginalFunction;
  result = originalFunc(arg1, imageBase, imageSize, arg4, arg5);
  
  SerialPrintHex("[*] Original returned", result);
  SerialPrint("************************************************\n\n");
  
  //
  // Note: We do NOT re-apply the patch since we only need one interception
  //
  
  return result;
}
