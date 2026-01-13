#include "Compiler.h"
#include "ExtendedHv.h"
#include "WinDefines.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern EFI_STATUS InstallPatch(volatile patchinfo_t *info, void *originalFunction, void *targetFunction);

// Public Globals
BOOLEAN gExtendedAllocation = FALSE;

// Public Functions
EFI_STATUS InstallPatch_BlImgAllocateImageBuffer(IN VOID *originalFunction);

// Private Globals
static volatile patchinfo_t gBlImgAllocateImageBufferPatchInfo;

// Private Functions
// None

// Private Functions
static UINT64 EFIAPI PatchedBlImgAllocateImageBuffer(
  VOID** imageBuffer, 
  UINTN imageSize, 
  UINT32 memoryType, 
  UINT32 attributes, 
  VOID* unknown1, 
  VOID* unknown2
);

// Implementation

EFI_STATUS InstallPatch_BlImgAllocateImageBuffer(IN VOID *originalFunction) {
  EFI_STATUS status;
  
  SerialPrint("[*] Installing BlImgAllocateImageBuffer patch\n");
  SerialPrintHex("  Target", (UINT64)originalFunction);
  
  status = InstallPatch(
    &gBlImgAllocateImageBufferPatchInfo, 
    originalFunction, 
    PatchedBlImgAllocateImageBuffer
  );
  
  if (EFI_ERROR(status)) {
    SerialPrint("[!] InstallPatch failed: %r\n", status);
  } else {
    SerialPrint("[+] BlImgAllocateImageBuffer patch installed\n");
  }
  
  return status;
}

static UINT64 EFIAPI PatchedBlImgAllocateImageBuffer(
  VOID** imageBuffer, 
  UINTN imageSize, 
  UINT32 memoryType, 
  UINT32 attributes, 
  VOID* unknown1, 
  VOID* unknown2
) {
  //
  // Check if this is the hypervisor image allocation
  // The hypervisor has a specific attribute value
  //
  if (attributes == ATTRIBUTE_HV_IMAGE && !gExtendedAllocation) {
    UINTN payloadSize = 100; // TODO: Get payload size...
    
    //
    // Extend allocation to include our payload
    //
    imageSize += payloadSize;
    
    //
    // Change memory type to RWX
    // This ensures our payload code can execute
    //
    memoryType = MEMORY_ATTRIBUTE_RWX;
    
    //
    // Set flag to prevent extending multiple times
    //
    gExtendedAllocation = TRUE;
  }
  
  //
  // Call original function via trampoline
  //
  const UINT64 result = 
    (
      (UINT64 (EFIAPI *)(VOID**, UINTN, UINT32, UINT32, VOID*, VOID*))
      gBlImgAllocateImageBufferPatchInfo.trampoline
    )
    (imageBuffer, imageSize, memoryType, attributes, unknown1, unknown2)
  ;
  
  return result;
}
