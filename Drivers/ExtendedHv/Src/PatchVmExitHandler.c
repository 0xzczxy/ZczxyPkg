#include "ExtendedHv.h"
#include "Compiler.h"
#include "WinDefines.h"

//
// Payload is separately compiled, the raw bytes are then included here
// 
#include "../Payload/payload_data.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern UINT64 PeAddSection(IN UINT64 imageBase, IN CONST CHAR8* sectionName, IN UINT32 virtualSize, IN UINT32 characteristics);
extern UINT64 FindPatternImage(IN VOID* imageBase, IN CONST CHAR8* pattern);

// Public Globals
// None

// Public Functions
EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize);
UINT64 PatchSizeVmExitHandler(VOID);

// Private Globals
// None

// Private Functions
// None

// Implementation

UINT64 PatchSizeVmExitHandler(VOID) {
  return PAYLOAD_SIZE;
}

EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize) {
  UINT64 section = 0;
  UINT64 scan = 0;
  
  //
  // Add new section to store our patched function
  // 
  section = PeAddSection(imageBase, ".zczxyhc", PatchSizeVmExitHandler(), SECTION_RWX);
  if (!section) {
    SerialPrint("[!] Failed to add section .zczxyhc for our custom payload!\n");
    return EFI_UNSUPPORTED;
  }

  //
  // Pattern scan for the call instruction (E8 = call with rel32 offset)
  // 
  scan = FindPatternImage((VOID*)imageBase, "E8 ? ? ? ? 48 89 04 24 E9");
  if (!scan) {
    SerialPrint("[!] Failed to find pattern for the call instruction.\n");
    return EFI_UNSUPPORTED;
  }
  
  SerialPrint("[+] Found call instruction at 0x%p\n", scan);
  
  //
  // Copy payload to the new section
  //
  CopyMem((VOID*)section, g_payload_data, PAYLOAD_SIZE);
  SerialPrint("[+] Copied payload (%u bytes) to section at 0x%p\n", PAYLOAD_SIZE, section);
  
  //
  // Calculate addresses for patching the call instruction
  // E8 xx xx xx xx = call rel32 (1 byte opcode + 4 byte offset)
  //
  const UINT64 callInstructionBase = scan + 5;  // Address after the call instruction
  const INT32 originalOffset = *(INT32*)(scan + 1);  // Read current rel32 offset
  const UINT64 originalFunction = callInstructionBase + originalOffset;
  const UINT64 hookedFunction = section + PAYLOAD_FUNCTION_OFFSET;  // Our hook at offset 0x10
  const INT32 newOffset = (INT32)(hookedFunction - callInstructionBase);
  
  //
  // Calculate offset from hooked function back to original
  // The payload needs this to call the original handler
  //
  const INT64 offsetToOriginal = (INT64)(originalFunction - hookedFunction);
  
  SerialPrintHex("  Call instruction", scan);
  SerialPrintHex("  Original function", originalFunction);
  SerialPrintHex("  Hooked function", hookedFunction);
  SerialPrint("  Original offset: %d\n", originalOffset);
  SerialPrint("  New offset: %d\n", newOffset);
  SerialPrint("  Offset to original: %lld\n", offsetToOriginal);
  
  //
  // Patch the G_original_offset_from_hook global in the payload
  // This is at offset 0x00 in the payload
  //
  INT64* globalOffset = PAYLOAD_GLOBAL_PTR((VOID*)section);
  *globalOffset = offsetToOriginal;
  SerialPrint("[+] Patched G_original_offset_from_hook in payload\n");
  
  //
  // Patch the call instruction to jump to our hooked function
  //
  DisableMemoryProtection();
  *(INT32*)(scan + 1) = newOffset;
  FlushInstructionCache((VOID*)scan, 5);
  EnableMemoryProtection();
  
  SerialPrint("[+] Patched call instruction to redirect to hook\n");
  SerialPrint("[+] VM Exit Handler hook installed successfully\n");
  
  return EFI_SUCCESS;
}
