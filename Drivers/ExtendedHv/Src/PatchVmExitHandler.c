#include "ExtendedHv.h"
#include "Compiler.h"
#include "WinDefines.h"

//
// Check Payload/README.md for instructions to compile
// 

#include "../Payload/payload_data.h"

#define HV_ARCH_UNKNOWN 0
#define HV_ARCH_INTEL 1
#define HV_ARCH_AMD 2

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
static int DetectHyperVArchitecture(IN VOID* imageBase, IN UINT64 imageSize);
static EFI_STATUS PatchVmExitHandler_Intel(IN UINT64 imageBase, IN UINT64 section);
static EFI_STATUS PatchVmExitHandler_Amd(IN UINT64 imageBase, IN UINT64 section);
static UINT64 FindCallBeforeInstruction(IN UINT64 instructionAddr, IN UINT64 searchRadius);


// Implementation
UINT64 PatchSizeVmExitHandler(VOID) {
  return PAYLOAD_SIZE;
}

static int DetectHyperVArchitecture(IN VOID* imageBase, IN UINT64 imageSize) {
  // AMD detection: Look for VMRUN instruction (0F 01 D8)
  if (FindPatternImage(imageBase, "0F 01 D8") != 0) {
    SerialPrint("[*] Detected AMD Hyper-V (hvax64.exe) - VMRUN instruction found\n");
    return HV_ARCH_AMD;
  }
  
  // Intel detection: Look for VMRESUME (0F 01 C3) or VMREAD (0F 78)
  if (FindPatternImage(imageBase, "0F 01 C3") != 0 || FindPatternImage(imageBase, "0F 78") != 0) {
    SerialPrint("[*] Detected Intel Hyper-V (hvix64.exe) - VM instructions found\n");
    return HV_ARCH_INTEL;
  }
  
  return HV_ARCH_UNKNOWN;
}

//
// Helper function to find CALL instruction before a given address
// Searches backwards from instructionAddr for E8 (CALL rel32)
//
static UINT64 FindCallBeforeInstruction(IN UINT64 instructionAddr, IN UINT64 searchRadius) {
  UINT64 searchStart = (instructionAddr > searchRadius) ? (instructionAddr - searchRadius) : 0;
  UINT64 lastCall = 0;
  
  for (UINT64 offset = searchStart; offset < instructionAddr; offset++) {
    if (*(UINT8*)offset == 0xE8) {  // CALL rel32 opcode
      lastCall = offset;
    }
  }
  
  return lastCall;
}

//
// Intel VM Exit Handler Patching
// Multiple pattern strategies based on the article
//
static EFI_STATUS PatchVmExitHandler_Intel(IN UINT64 imageBase, IN UINT64 section) {
  UINT64 scan = 0;
  UINT64 callScan = 0;
  
  SerialPrint("[*] Attempting Intel-specific VM exit handler patch...\n");
  
  //
  // Strategy 1: Search for VMRESUME (0F 01 C3) and look backwards for CALL
  // This is the most reliable method according to the article
  //
  SerialPrint("[*] Strategy 1: Searching for VMRESUME instruction...\n");
  scan = FindPatternImage((VOID*)imageBase, "0F 01 C3");
  
  if (scan) {
    SerialPrint("[+] Found VMRESUME at 0x%p\n", scan);
    
    // Look backwards for CALL instruction (typically within 256 bytes)
    callScan = FindCallBeforeInstruction(scan, 256);
    
    if (callScan) {
      SerialPrint("[+] Found CALL before VMRESUME at 0x%p\n", callScan);
      goto patch_call;
    }
    SerialPrint("[!] No CALL found before VMRESUME, trying next strategy...\n");
  }
  
  //
  // Strategy 2: Search for VMCS_HOST_RIP field (0x6C16)
  // Then look for nearby VMWRITE instruction and CALL
  //
  SerialPrint("[*] Strategy 2: Searching for VMCS_HOST_RIP (0x6C16)...\n");
  scan = FindPatternImage((VOID*)imageBase, "66 B9 16 6C");  // mov cx, 0x6C16
  
  if (!scan) {
    scan = FindPatternImage((VOID*)imageBase, "41 B9 16 6C 00 00");  // mov r9d, 0x6C16
  }
  
  if (scan) {
    SerialPrint("[+] Found VMCS_HOST_RIP reference at 0x%p\n", scan);
    
    // Look forward for VMWRITE (0F 79) then backwards for CALL
    UINT64 vmwriteScan = 0;
    for (UINT64 offset = scan; offset < scan + 512; offset++) {
      if (*(UINT16*)offset == 0x790F) {  // VMWRITE
        vmwriteScan = offset;
        break;
      }
    }
    
    if (vmwriteScan) {
      SerialPrint("[+] Found VMWRITE at 0x%p\n", vmwriteScan);
      callScan = FindCallBeforeInstruction(vmwriteScan, 256);
      
      if (callScan) {
        SerialPrint("[+] Found CALL before VMWRITE at 0x%p\n", callScan);
        goto patch_call;
      }
    }
    SerialPrint("[!] Could not locate CALL, trying next strategy...\n");
  }
  
  //
  // Strategy 3: Original method - VMREAD and backwards scan
  //
  SerialPrint("[*] Strategy 3: Searching for VMREAD instruction...\n");
  scan = FindPatternImage((VOID*)imageBase, "0F 78");
  
  if (scan) {
    SerialPrint("[+] Found VMREAD at 0x%p\n", scan);
    callScan = FindCallBeforeInstruction(scan, 256);
    
    if (callScan) {
      SerialPrint("[+] Found CALL before VMREAD at 0x%p\n", callScan);
      goto patch_call;
    }
  }
  
  SerialPrint("[!] All Intel strategies exhausted. Unable to locate VM exit handler.\n");
  return EFI_UNSUPPORTED;

patch_call:
  //
  // Calculate addresses for patching
  //
  const UINT64 callInstructionBase = callScan + 5;
  const INT32 originalOffset = *(INT32*)(callScan + 1);
  const UINT64 originalFunction = callInstructionBase + originalOffset;
  const UINT64 hookedFunction = section + PAYLOAD_FUNCTION_OFFSET;
  const INT32 newOffset = (INT32)(hookedFunction - callInstructionBase);
  const INT64 offsetToOriginal = (INT64)(originalFunction - hookedFunction);
  
  SerialPrintHex("  Call instruction", callScan);
  SerialPrintHex("  Original function", originalFunction);
  SerialPrintHex("  Hooked function", hookedFunction);
  SerialPrint("  Original offset: %d\n", originalOffset);
  SerialPrint("  New offset: %d\n", newOffset);
  SerialPrint("  Offset to original: %lld\n", offsetToOriginal);
  
  //
  // Patch the payload's global offset
  //
  INT64 *globalOffset = PAYLOAD_GLOBAL_PTR((VOID*)section);
  *globalOffset = offsetToOriginal;
  SerialPrint("[+] Patched G_original_offset_from_hook in payload\n");

  //
  // Patch the payload's arch offset
  //
  INT32 *archOffset = PAYLOAD_ARCH_PTR((VOID*)section);
  *archOffset = 1; // ARCH_INTEL Payload/src/main.c
  SerialPrint("[+] Patched G_arch to intel in payload\n");
  
  //
  // Patch the CALL instruction
  //
  DisableMemoryProtection();
  *(INT32*)(callScan + 1) = newOffset;
  FlushInstructionCache((VOID*)callScan, 5);
  EnableMemoryProtection();
  
  SerialPrint("[+] Successfully patched Intel VM exit handler\n");
  return EFI_SUCCESS;
}

//
// AMD VM Exit Handler Patching
// Multiple pattern strategies based on the article
//
static EFI_STATUS PatchVmExitHandler_Amd(IN UINT64 imageBase, IN UINT64 section) {
  UINT64 scan = 0;
  UINT64 callScan = 0;
  
  SerialPrint("[*] Attempting AMD-specific VM exit handler patch...\n");
  
  //
  // Strategy 1: Search for VMRUN (0F 01 D8) and look backwards for CALL
  // According to the article, vmexit handler should be near VMRUN
  //
  SerialPrint("[*] Strategy 1: Searching for VMRUN instruction...\n");
  scan = FindPatternImage((VOID*)imageBase, "0F 01 D8");
  
  if (scan) {
    SerialPrint("[+] Found VMRUN at 0x%p\n", scan);
    
    // Look backwards for CALL instruction (typically within 256 bytes)
    callScan = FindCallBeforeInstruction(scan, 256);
    
    if (callScan) {
      SerialPrint("[+] Found CALL before VMRUN at 0x%p\n", callScan);
      goto patch_call;
    }
    SerialPrint("[!] No CALL found before VMRUN, trying next strategy...\n");
  }
  
  //
  // Strategy 2: Look for VMCB exit code access (offset 0x70)
  // Pattern: mov reg, [reg+70h] or similar
  //
  SerialPrint("[*] Strategy 2: Searching for VMCB exit code access...\n");
  
  // Look for: mov r*, [r*+70h]
  const CHAR8* vmcbPatterns[] = {
    "48 8B ? 70",           // mov r64, [r*+70h]
    "4C 8B ? 70",           // mov r64, [r*+70h] (r8-r15)
    "48 8B 80 70 00 00 00", // mov rax, [rax+70h]
    "4C 8B 80 70 00 00 00", // mov r8, [rax+70h]
    NULL
  };
  
  for (INT32 i = 0; vmcbPatterns[i] != NULL; i++) {
    scan = FindPatternImage((VOID*)imageBase, vmcbPatterns[i]);
    if (scan) {
      SerialPrint("[+] Found VMCB exit code access at 0x%p\n", scan);
      
      // Look backwards for CALL
      callScan = FindCallBeforeInstruction(scan, 512);
      if (callScan) {
        SerialPrint("[+] Found CALL before VMCB access at 0x%p\n", callScan);
        goto patch_call;
      }
    }
  }
  
  //
  // Strategy 3: VMLOAD -> VMRUN -> VMSAVE sequence
  // Look for this sequence and find CALL in between
  //
  SerialPrint("[*] Strategy 3: Searching for VMLOAD/VMRUN/VMSAVE sequence...\n");
  
  // VMLOAD (0F 01 DA), then scan for CALL before VMRUN
  UINT64 vmloadScan = FindPatternImage((VOID*)imageBase, "0F 01 DA");
  if (vmloadScan) {
    SerialPrint("[+] Found VMLOAD at 0x%p\n", vmloadScan);
    
    // Look for VMRUN after VMLOAD (within 512 bytes)
    for (UINT64 offset = vmloadScan; offset < vmloadScan + 512; offset++) {
      if (*(UINT8*)offset == 0x0F && *(UINT16*)(offset + 1) == 0xD801) {  // VMRUN
        SerialPrint("[+] Found VMRUN at 0x%p after VMLOAD\n", offset);
        
        // Look for CALL between VMLOAD and VMRUN
        for (UINT64 callOffset = vmloadScan; callOffset < offset; callOffset++) {
          if (*(UINT8*)callOffset == 0xE8) {
            callScan = callOffset;
            SerialPrint("[+] Found CALL in VMLOAD/VMRUN sequence at 0x%p\n", callScan);
            goto patch_call;
          }
        }
        break;
      }
    }
  }
  
  //
  // Strategy 4: Original pattern (fallback)
  //
  SerialPrint("[*] Strategy 4: Using original pattern...\n");
  scan = FindPatternImage((VOID*)imageBase, "E8 ? ? ? ? 48 89 04 24 E9");
  
  if (scan) {
    SerialPrint("[+] Found original pattern at 0x%p\n", scan);
    callScan = scan;
    goto patch_call;
  }
  
  SerialPrint("[!] All AMD strategies exhausted. Unable to locate VM exit handler.\n");
  return EFI_UNSUPPORTED;

patch_call:
  //
  // Calculate addresses for patching
  //
  const UINT64 callInstructionBase = callScan + 5;
  const INT32 originalOffset = *(INT32*)(callScan + 1);
  const UINT64 originalFunction = callInstructionBase + originalOffset;
  const UINT64 hookedFunction = section + PAYLOAD_FUNCTION_OFFSET;
  const INT32 newOffset = (INT32)(hookedFunction - callInstructionBase);
  const INT64 offsetToOriginal = (INT64)(originalFunction - hookedFunction);
  
  SerialPrintHex("  Call instruction", callScan);
  SerialPrintHex("  Original function", originalFunction);
  SerialPrintHex("  Hooked function", hookedFunction);
  SerialPrint("  Original offset: %d\n", originalOffset);
  SerialPrint("  New offset: %d\n", newOffset);
  SerialPrint("  Offset to original: %lld\n", offsetToOriginal);
  
  //
  // Patch the payload's global offset
  //
  INT64 *globalOffset = PAYLOAD_GLOBAL_PTR((VOID*)section);
  *globalOffset = offsetToOriginal;
  SerialPrint("[+] Patched G_original_offset_from_hook in payload\n");

  //
  // Patch the payload's arch offset
  //
  INT32 *archOffset = PAYLOAD_ARCH_PTR((VOID*)section);
  *archOffset = 2; // ARCH_AMD Payload/src/main.c
  SerialPrint("[+] Patched G_arch to amd in payload\n");
  
  //
  // Patch the CALL instruction
  //
  DisableMemoryProtection();
  *(INT32*)(callScan + 1) = newOffset;
  FlushInstructionCache((VOID*)callScan, 5);
  EnableMemoryProtection();
  
  SerialPrint("[+] Successfully patched AMD VM exit handler\n");
  return EFI_SUCCESS;
}

//
// Main patching function - handles both architectures
//
EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize) {
  UINT64 section = 0;
  int arch = HV_ARCH_UNKNOWN;
  EFI_STATUS status = EFI_UNSUPPORTED;
  
  SerialPrint("\n");
  SerialPrint("================================================\n");
  SerialPrint("  VM Exit Handler Patching\n");
  SerialPrint("================================================\n");
  
  //
  // Detect the Hyper-V architecture
  //
  arch = DetectHyperVArchitecture((VOID*)imageBase, imageSize);
  if (arch == HV_ARCH_UNKNOWN) {
    SerialPrint("[!] Failed to detect Hyper-V architecture (Intel/AMD).\n");
    return EFI_UNSUPPORTED;
  }
  
  //
  // Add new section to store our payload
  //
  section = PeAddSection(imageBase, ".zczxyhc", PatchSizeVmExitHandler(), SECTION_RWX);
  if (!section) {
    SerialPrint("[!] Failed to add section .zczxyhc for payload!\n");
    return EFI_UNSUPPORTED;
  }
  
  //
  // Copy payload to the new section
  //
  CopyMem((VOID*)section, g_payload_data, PAYLOAD_SIZE);
  SerialPrint("[+] Copied payload (%u bytes) to section at 0x%p\n", PAYLOAD_SIZE, section);
  
  //
  // Apply architecture-specific patching with multiple strategies
  //
  if (arch == HV_ARCH_INTEL) {
    status = PatchVmExitHandler_Intel(imageBase, section);
  } else if (arch == HV_ARCH_AMD) {
    status = PatchVmExitHandler_Amd(imageBase, section);
  }
  
  if (status != EFI_SUCCESS) {
    SerialPrint("[!] Failed to apply patches.\n");
    SerialPrint("================================================\n\n");
    return status;
  }

  if (arch == HV_ARCH_INTEL) {
    SerialPrint("\n[+] VM Exit Handler successfully hooked Intel\n");
  } else {
    SerialPrint("\n[+] VM Exit Handler successfully hooked AMD\n");
  }
  
  return EFI_SUCCESS;
}
