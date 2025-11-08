#include "ExtendedHv.h"

// Imports
extern UINT64 DisableWriteProtection(VOID);
extern VOID RestoreWriteProtection(UINT64 originalCr0);

// Internal Types
typedef struct _LDR_DATA_TABLE_ENTRY {
   LIST_ENTRY InLoadOrderLinks;
   LIST_ENTRY InMemoryOrderLinks;
   LIST_ENTRY InInitializationOrderLinks;
   VOID *DllBase;
   VOID *EntryPoint;
   UINTN SizeOfImage;
} LDR_DATA_TABLE_ENTRY;

// No __fastcall needed - x64 uses fastcall by default
typedef UINT64 (*func_BlLdrLoadImage)(
    VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*,  // 1-8
    VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*,  // 9-16
    VOID*, VOID*, VOID*, VOID*                               // 17-20 (extra for future-proofing)
);

// Public Functions
EFI_STATUS PatchWinload(IN VOID *imageBase, IN UINT64 imageSize);

// Private Functions
static UINT64 PatchedBlLdrLoadImage(
    VOID *arg1, VOID *arg2, VOID *arg3, VOID *arg4,
    VOID *arg5, VOID *arg6, VOID *arg7, VOID *arg8,
    VOID *arg9, VOID *arg10, VOID *arg11, VOID *arg12,
    VOID *arg13, VOID *arg14, VOID *arg15, VOID *arg16,
    VOID *arg17, VOID *arg18, VOID *arg19, VOID *arg20
);
static VOID *FindExportByName(VOID *imageBase, const char *exportName);
static UINT8 *FindFreeSpaceInTextSection(VOID *imageBase, UINTN *outRva);
static EFI_STATUS PatchExportTable(VOID *imageBase, const char *exportName, UINT32 newRva);

// Private Globals
static func_BlLdrLoadImage gOriginalBlLdrLoadImage = NULL;
static UINT32 gCallCount = 0;



EFI_STATUS PatchWinload(IN VOID *imageBase, IN UINT64 imageSize) {
    SerialPrint(
        "\n"
        "========================================\n"
        "  Patching Winload\n"
        "========================================\n"
    );

    //
    // Find BlLdrLoadImage via export table
    // 
    SerialPrint("[*] Searching for BlLdrLoadImage export...\n");
    VOID *originalFunc = FindExportByName(imageBase, "BlLdrLoadImage");

    if (!originalFunc) {
        SerialPrint("[-] BlLdrLoadImage export not found\n");
        return EFI_NOT_FOUND;
    }

    SerialPrint("[+] Found BlLdrLoadImage at: 0x%llx\n", (UINT64)originalFunc);
    gOriginalBlLdrLoadImage = (func_BlLdrLoadImage)originalFunc;

    //
    // Find free space in .text section
    // 
    SerialPrint("[*] Locating free space in .text section...\n");
    UINTN trampolineRva;
    UINT8 *trampolineAddr = FindFreeSpaceInTextSection(imageBase, &trampolineRva);
    
    if (!trampolineAddr) {
        SerialPrint("[-] No free space in .text section\n");
        return EFI_OUT_OF_RESOURCES;
    }

    SerialPrint("[+] Trampoline location: 0x%llx (RVA: 0x%llx)\n", 
                (UINT64)trampolineAddr, trampolineRva);

    //
    // Write trampoline code
    // 
    SerialPrint("[*] Writing trampoline...\n");
    UINT64 cr0 = DisableWriteProtection();

    //
    // Trampoline: mov rax, <PatchedBlLdrLoadImage>; jmp rax
    // 
    trampolineAddr[0] = 0x48;
    trampolineAddr[1] = 0xB8;
    *(UINT64 *)&trampolineAddr[2] = (UINT64)&PatchedBlLdrLoadImage;
    trampolineAddr[10] = 0xFF;
    trampolineAddr[11] = 0xE0;
    
    RestoreWriteProtection(cr0);
    SerialPrint("[+] Trampoline written\n");

    //
    // Patch export table
    // 
    SerialPrint("[*] Patching export table...\n");
    EFI_STATUS status = PatchExportTable(imageBase, "BlLdrLoadImage", (UINT32)trampolineRva);
    
    if (EFI_ERROR(status)) {
        SerialPrint("[-] Failed to patch export table\n");
        return status;
    }

    SerialPrint("[+] Hook installed successfully\n");
    return EFI_SUCCESS;
}

static UINT64 PatchedBlLdrLoadImage(
    VOID *arg1, VOID *arg2, VOID *arg3, VOID *arg4,
    VOID *arg5, VOID *arg6, VOID *arg7, VOID *arg8,
    VOID *arg9, VOID *arg10, VOID *arg11, VOID *arg12,
    VOID *arg13, VOID *arg14, VOID *arg15, VOID *arg16,
    VOID *arg17, VOID *arg18, VOID *arg19, VOID *arg20
) {
    gCallCount++;
    
    SerialPrint("\n[*] === BlLdrLoadImage HOOK CALLED (Call #%d) ===\n", gCallCount);
    SerialPrint("[*] arg1: 0x%llx, arg2: 0x%llx\n", (UINT64)arg1, (UINT64)arg2);
    
    // Check if arg2 is a path (it should be UTF-16 string)
    if (arg2 != NULL) {
        UINT16 *path = (UINT16 *)arg2;
        SerialPrint("[*] arg2 (path): ");
        for (UINTN i = 0; i < 64 && path[i] != 0; i++) {
            if (path[i] < 128) {
                SerialPrint("%c", (char)path[i]);
            }
        }
        SerialPrint("\n");
    }
    
    //
    // Call original
    //  
    UINT64 status = gOriginalBlLdrLoadImage(
        arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8,
        arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
        arg17, arg18, arg19, arg20
    );

    SerialPrint("[*] BlLdrLoadImage returned: 0x%llx\n", status);
    SerialPrint("[*] arg8 pointer: 0x%llx\n", (UINT64)arg8);

    if (arg8 != NULL) {
        LDR_DATA_TABLE_ENTRY **ldrEntryPtr = (LDR_DATA_TABLE_ENTRY **)arg8;
        SerialPrint("[*] *arg8 (LDR_DATA_TABLE_ENTRY*): 0x%llx\n", (UINT64)(*ldrEntryPtr));
        
        if (*ldrEntryPtr != NULL) {
            LDR_DATA_TABLE_ENTRY *ldrEntry = *ldrEntryPtr;
            
            SerialPrint("[+] Module loaded successfully:\n");
            SerialPrint("    DllBase: 0x%llx\n", (UINT64)ldrEntry->DllBase);
            SerialPrint("    EntryPoint: 0x%llx\n", (UINT64)ldrEntry->EntryPoint);
            SerialPrint("    SizeOfImage: 0x%llx\n", ldrEntry->SizeOfImage);

            // Check for HvImageInfo export
            VOID *hvImageInfo = FindExportByName(ldrEntry->DllBase, "HvImageInfo");
            if (hvImageInfo != NULL) {
                SerialPrint("\n[!!!] FOUND SECUREKERNEL.EXE!\n");
                SerialPrint("[!!!] HvImageInfo export at: 0x%llx\n", (UINT64)hvImageInfo);
                SerialPrint("\n");
            }
        }
    }

    SerialPrint("[*] === End Hook Call #%d ===\n\n", gCallCount);
    return status;
}

static VOID *FindExportByName(VOID *imageBase, const char *exportName) {
    EFI_IMAGE_DOS_HEADER *dosHeader = (EFI_IMAGE_DOS_HEADER *)imageBase;
    EFI_IMAGE_NT_HEADERS64 *ntHeaders = (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)imageBase + dosHeader->e_lfanew);
    
    UINT32 exportDirRva = ntHeaders->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportDirRva == 0) {
        return NULL;
    }

    EFI_IMAGE_EXPORT_DIRECTORY *exportDir = (EFI_IMAGE_EXPORT_DIRECTORY *)((UINT8 *)imageBase + exportDirRva);
    UINT32 *nameTable = (UINT32 *)((UINT8 *)imageBase + exportDir->AddressOfNames);
    UINT16 *ordinalTable = (UINT16 *)((UINT8 *)imageBase + exportDir->AddressOfNameOrdinals);
    UINT32 *functionTable = (UINT32 *)((UINT8 *)imageBase + exportDir->AddressOfFunctions);

    for (UINT32 i = 0; i < exportDir->NumberOfNames; i++) {
        const char *name = (const char *)((UINT8 *)imageBase + nameTable[i]);
        
        // Compare strings manually (no strcmp available)
        BOOLEAN match = TRUE;
        for (UINTN j = 0; exportName[j] != '\0' || name[j] != '\0'; j++) {
            if (exportName[j] != name[j]) {
                match = FALSE;
                break;
            }
        }

        if (match) {
            UINT16 ordinal = ordinalTable[i];
            UINT32 rva = functionTable[ordinal];
            return (VOID *)((UINT8 *)imageBase + rva);
        }
    }

    return NULL;
}

static UINT8 *FindFreeSpaceInTextSection(VOID *imageBase, UINTN *outRva) {
    EFI_IMAGE_DOS_HEADER *dosHeader = (EFI_IMAGE_DOS_HEADER *)imageBase;
    EFI_IMAGE_NT_HEADERS64 *ntHeaders = (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)imageBase + dosHeader->e_lfanew);
    
    EFI_IMAGE_SECTION_HEADER *sections = (EFI_IMAGE_SECTION_HEADER *)((UINT8 *)&ntHeaders->OptionalHeader + 
                                          ntHeaders->FileHeader.SizeOfOptionalHeader);
    
    for (UINT16 i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        // Check if this is .text section
        BOOLEAN isTextSection = TRUE;
        const char textName[] = ".text";
        for (UINTN j = 0; j < 6; j++) {
            if (sections[i].Name[j] != textName[j]) {
                isTextSection = FALSE;
                break;
            }
        }

        if (isTextSection) {
            // Calculate aligned size
            UINT32 alignedSize = (sections[i].Misc.VirtualSize + ntHeaders->OptionalHeader.SectionAlignment - 1) 
                                & ~(ntHeaders->OptionalHeader.SectionAlignment - 1);
            
            UINT32 freeSpace = alignedSize - sections[i].Misc.VirtualSize;
            
            SerialPrint("[*] .text section: VirtualSize=0x%x, Aligned=0x%x, Free=0x%x\n",
                        sections[i].Misc.VirtualSize, alignedSize, freeSpace);

            // Need at least 12 bytes for trampoline
            if (freeSpace >= 12) {
                UINT32 rva = sections[i].VirtualAddress + sections[i].Misc.VirtualSize;
                *outRva = rva;
                return (UINT8 *)imageBase + rva;
            }
        }
    }

    return NULL;
}

static EFI_STATUS PatchExportTable(VOID *imageBase, const char *exportName, UINT32 newRva) {
    EFI_IMAGE_DOS_HEADER *dosHeader = (EFI_IMAGE_DOS_HEADER *)imageBase;
    EFI_IMAGE_NT_HEADERS64 *ntHeaders = (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)imageBase + dosHeader->e_lfanew);
    
    UINT32 exportDirRva = ntHeaders->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportDirRva == 0) {
        return EFI_NOT_FOUND;
    }

    EFI_IMAGE_EXPORT_DIRECTORY *exportDir = (EFI_IMAGE_EXPORT_DIRECTORY *)((UINT8 *)imageBase + exportDirRva);
    UINT32 *nameTable = (UINT32 *)((UINT8 *)imageBase + exportDir->AddressOfNames);
    UINT16 *ordinalTable = (UINT16 *)((UINT8 *)imageBase + exportDir->AddressOfNameOrdinals);
    UINT32 *functionTable = (UINT32 *)((UINT8 *)imageBase + exportDir->AddressOfFunctions);

    for (UINT32 i = 0; i < exportDir->NumberOfNames; i++) {
        const char *name = (const char *)((UINT8 *)imageBase + nameTable[i]);
        
        BOOLEAN match = TRUE;
        for (UINTN j = 0; exportName[j] != '\0' || name[j] != '\0'; j++) {
            if (exportName[j] != name[j]) {
                match = FALSE;
                break;
            }
        }

        if (match) {
            UINT16 ordinal = ordinalTable[i];
            UINT32 oldRva = functionTable[ordinal];
            
            SerialPrint("[*] Found export at ordinal %d, old RVA: 0x%x\n", ordinal, oldRva);
            
            UINT64 cr0 = DisableWriteProtection();
            functionTable[ordinal] = newRva;
            RestoreWriteProtection(cr0);
            
            SerialPrint("[+] Patched to new RVA: 0x%x\n", newRva);
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}
