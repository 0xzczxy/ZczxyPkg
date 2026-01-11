#include "ExtendedHv.h"

//
// Pattern to find VM exit handler call in hv.exe
// Pattern: E8 ? ? ? ? 48 89 04 24 E9 (call followed by mov [rsp], rax; jmp)
//
static UINT8 gVmExitPattern[] = {
    0xE8, 0xFF, 0xFF, 0xFF, 0xFF,  // call <offset>
    0x48, 0x89, 0x04, 0x24,         // mov [rsp], rax
    0xE9                            // jmp
};

static UINT8 gVmExitMask[] = {
    0xFF, 0x00, 0x00, 0x00, 0x00,  // Match E8, ignore offset
    0xFF, 0xFF, 0xFF, 0xFF,         // Match exactly
    0xFF                            // Match exactly
};

// Imports
extern VOID *FindPatternPeExecutableSections(
    IN VOID *imageBase, IN UINT64 imageSize,
    IN VOID *pattern, IN VOID *mask,
    IN UINT64 pattern_size
);

// Public Globals
// None

// Public Functions
EFI_STATUS PatchHvImage(IN VOID *imageBase, IN UINTN imageSize);

// Private Globals
static UINT64 gVmExitHandlerAddress = 0;
static UINT64 gVmExitCallSite = 0;

// Private Functions
static UINT64 CalculateCallTarget(UINT8 *callInstruction);
static VOID DumpBytes(UINT8 *address, UINTN count);


EFI_STATUS PatchHvImage(IN VOID *imageBase, IN UINTN imageSize) {
    SerialPrint(
        "\n"
        "========================================\n"
        "  Patching HV.EXE\n"
        "========================================\n"
    );
    SerialPrintHex("Image Base", (UINT64)imageBase);
    SerialPrintHex("Image Size", imageSize);

    //
    // Validate PE structure
    //
    EFI_IMAGE_DOS_HEADER *dosHeader = (EFI_IMAGE_DOS_HEADER *)imageBase;
    if (dosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE) {
        SerialPrint("[-] Invalid DOS signature\n");
        return EFI_INVALID_PARAMETER;
    }

    EFI_IMAGE_NT_HEADERS64 *ntHeaders = 
        (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)imageBase + dosHeader->e_lfanew);
    
    if (ntHeaders->Signature != EFI_IMAGE_NT_SIGNATURE) {
        SerialPrint("[-] Invalid NT signature\n");
        return EFI_INVALID_PARAMETER;
    }

    //
    // Search for VM exit handler call pattern
    //
    SerialPrint("[*] Searching for VM exit handler call site...\n");
    SerialPrint("[*] Pattern: E8 ?? ?? ?? ?? 48 89 04 24 E9\n");
    
    VOID *patternMatch = FindPatternPeExecutableSections(
        imageBase,
        imageSize,
        gVmExitPattern,
        gVmExitMask,
        sizeof(gVmExitPattern)
    );

    if (!patternMatch) {
        SerialPrint("[-] VM exit handler pattern not found\n");
        SerialPrint("[-] This may not be the correct hv.exe or pattern needs updating\n");
        return EFI_NOT_FOUND;
    }

    gVmExitCallSite = (UINT64)patternMatch;
    SerialPrintHex("[+] Found call site", gVmExitCallSite);

    //
    // Calculate the target of the call instruction
    //
    gVmExitHandlerAddress = CalculateCallTarget((UINT8 *)patternMatch);
    SerialPrintHex("[+] VM exit handler at", gVmExitHandlerAddress);

    //
    // Dump bytes around the call site for verification
    //
    SerialPrint("[*] Call site bytes:\n");
    DumpBytes((UINT8 *)patternMatch, 32);

    //
    // Dump bytes at the handler
    //
    SerialPrint("[*] Handler bytes:\n");
    DumpBytes((UINT8 *)gVmExitHandlerAddress, 32);

    //
    // At this point we've successfully located the VM exit handler
    // Next steps would be:
    // 1. Add a new section to hv.exe for our hook code
    // 2. Write the hook trampoline
    // 3. Modify the call to point to our hook
    //
    SerialPrint("[+] VM exit handler successfully located!\n");
    SerialPrint("[*] Ready for hook installation (not yet implemented)\n");
    SerialPrint("========================================\n\n");

    return EFI_SUCCESS;
}

static UINT64 CalculateCallTarget(UINT8 *callInstruction) {
    //
    // E8 instruction format: E8 [4-byte relative offset]
    // Target = (address after instruction) + offset
    //
    INT32 offset = *(INT32 *)(callInstruction + 1);
    UINT64 nextInstruction = (UINT64)callInstruction + 5;  // E8 + 4 bytes = 5 bytes
    
    SerialPrint("[*] Call offset: 0x%x (%d)\n", offset, offset);
    SerialPrintHex("[*] Next instruction", nextInstruction);
    
    return nextInstruction + offset;
}

static VOID DumpBytes(UINT8 *address, UINTN count) {
    SerialPrint("    ");
    for (UINTN i = 0; i < count; i++) {
        SerialPrint("%02x ", address[i]);
        if ((i + 1) % 16 == 0 && i + 1 < count) {
            SerialPrint("\n    ");
        }
    }
    SerialPrint("\n");
}
