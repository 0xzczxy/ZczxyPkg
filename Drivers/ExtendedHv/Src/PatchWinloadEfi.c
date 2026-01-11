#include "ExtendedHv.h"
#include "WinDefines.h"
#include <Library/PatchLib.h>
#include <Library/PatchWinLib.h>

typedef UINT64 (*func_BlLdrLoadImage)(
    VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*,
    VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*,
    VOID*, VOID*
);

// Imports
extern int StrCmp_(const CHAR16 *p1, const CHAR16 *p2);

// Public Globals
UINT64 gBlLdrLoadImageCallCount = 0;
BOOLEAN gHvDetected = FALSE;
UINT64 gHvImageBase = 0;
UINTN gHvImageSize = 0;

// Public Functions
EFI_STATUS PatchWinload(IN VOID *imageBase, IN UINT64 imageSize);

// Private Globals
static func_BlLdrLoadImage gOriginalBlLdrLoadImage = NULL;

// Private Functions
static UINT64 PatchedBlLdrLoadImage(
    VOID *arg1, VOID *arg2, VOID *arg3, VOID *arg4,
    VOID *arg5, VOID *arg6, VOID *arg7, VOID *arg8,
    VOID *arg9, VOID *arg10, VOID *arg11, VOID *arg12,
    VOID *arg13, VOID *arg14, VOID *arg15, VOID *arg16,
    VOID *arg17, VOID *arg18
);
static VOID *FindExportByName(VOID *imageBase, const char *exportName);
static UINT8 *FindFreeSpaceInTextSection(VOID *imageBase, UINTN *outRva);
static EFI_STATUS PatchExportTable(VOID *imageBase, const char *exportName, UINT32 newRva);


EFI_STATUS PatchWinload(IN VOID *imageBase, IN UINT64 imageSize) {
    VOID *originalFunc;
    UINTN trampolineRva;
    UINT8 *trampolineAddr;
    UINT64 patchAddr;
    EFI_STATUS status;

    SerialPrint(
        "\n"
        "========================================\n"
        "  Patching Winload\n"
        "========================================\n"
    );

    //
    // Find BlLdrLoadImage export
    // 
    SerialPrint("[*] Searching for BlLdrLoadImage export...\n");
    originalFunc = FindExportByName(imageBase, "BlLdrLoadImage");

    if (!originalFunc) {
        SerialPrint("[-] BlLdrLoadImage export not found\n");
        return EFI_NOT_FOUND;
    }

    SerialPrintHex("[+] Found BlLdrLoadImage", (UINT64)originalFunc);
    gOriginalBlLdrLoadImage = (func_BlLdrLoadImage)originalFunc;

    //
    // Find free space in .text section
    // 
    SerialPrint("[*] Locating free space in .text section...\n");
    trampolineAddr = FindFreeSpaceInTextSection(imageBase, &trampolineRva);
    
    if (!trampolineAddr) {
        SerialPrint("[-] No free space in .text section\n");
        return EFI_OUT_OF_RESOURCES;
    }

    SerialPrintHex("[+] Trampoline location", (UINT64)trampolineAddr);
    SerialPrintHex("[+] Trampoline RVA", trampolineRva);

    //
    // Write trampoline using PatchLib format
    // FF 25 00 00 00 00 [8 bytes address]
    // 
    SerialPrint("[*] Writing trampoline...\n");
    patchAddr = (UINT64)&PatchedBlLdrLoadImage;
    
    trampolineAddr[0] = 0xFF;
    trampolineAddr[1] = 0x25;
    trampolineAddr[2] = 0x00;
    trampolineAddr[3] = 0x00;
    trampolineAddr[4] = 0x00;
    trampolineAddr[5] = 0x00;
    CopyMem(&trampolineAddr[6], &patchAddr, sizeof(UINT64));
    
    SerialPrint("[+] Trampoline written\n");

    //
    // Patch export table
    // 
    SerialPrint("[*] Patching export table...\n");
    status = PatchExportTable(imageBase, "BlLdrLoadImage", (UINT32)trampolineRva);
    
    if (EFI_ERROR(status)) {
        SerialPrint("[-] Failed to patch export table\n");
        return status;
    }

    SerialPrint("[+] Export table patched\n");
    SerialPrint("[+] Hook installed successfully\n");
    SerialPrint("========================================\n\n");
    return EFI_SUCCESS;
}

static UINT64 PatchedBlLdrLoadImage(
    VOID *arg1, VOID *arg2, VOID *arg3, VOID *arg4,
    VOID *arg5, VOID *arg6, VOID *arg7, VOID *arg8,
    VOID *arg9, VOID *arg10, VOID *arg11, VOID *arg12,
    VOID *arg13, VOID *arg14, VOID *arg15, VOID *arg16,
    VOID *arg17, VOID *arg18
) {
    //
    // NO SerialPrint - watchdog will reboot
    //

    //
    // Increment Count
    // 
    gBlLdrLoadImageCallCount++;

    //
    // Call Original Function
    // 
    EFI_STATUS status = gOriginalBlLdrLoadImage(
        arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8,
        arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
        arg17, arg18
    );
    
    //
    // Ensure the function is correct
    // 
    if (EFI_ERROR(status)) return status;

    // CHAR16* imagePath = (CHAR16*)arg3;
    CHAR16* imageName = (CHAR16*)arg4;

    if (/*!imagePath ||*/ !imageName)
        return status;

    // const PLDR_DATA_TABLE_ENTRY entry = *(PPLDR_DATA_TABLE_ENTRY)arg9;

    if (StrCmp_(imageName, L"hv.exe")) return status;

     if (gHvDetected)
        return status;

    gHvDetected = TRUE;

    //
    // Call original and return
    //  
    return status;
}

static VOID *FindExportByName(VOID *imageBase, const char *exportName) {
    EFI_IMAGE_DOS_HEADER *dosHeader;
    EFI_IMAGE_NT_HEADERS64 *ntHeaders;
    EFI_IMAGE_EXPORT_DIRECTORY *exportDir;
    UINT32 *nameTable;
    UINT16 *ordinalTable;
    UINT32 *functionTable;
    UINT32 exportDirRva;
    UINT32 i;
    
    dosHeader = (EFI_IMAGE_DOS_HEADER *)imageBase;
    ntHeaders = (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)imageBase + dosHeader->e_lfanew);
    
    exportDirRva = ntHeaders->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportDirRva == 0) {
        return NULL;
    }

    exportDir = (EFI_IMAGE_EXPORT_DIRECTORY *)((UINT8 *)imageBase + exportDirRva);
    nameTable = (UINT32 *)((UINT8 *)imageBase + exportDir->AddressOfNames);
    ordinalTable = (UINT16 *)((UINT8 *)imageBase + exportDir->AddressOfNameOrdinals);
    functionTable = (UINT32 *)((UINT8 *)imageBase + exportDir->AddressOfFunctions);

    for (i = 0; i < exportDir->NumberOfNames; i++) {
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
            UINT32 rva = functionTable[ordinal];
            return (VOID *)((UINT8 *)imageBase + rva);
        }
    }

    return NULL;
}

static UINT8 *FindFreeSpaceInTextSection(VOID *imageBase, UINTN *outRva) {
    EFI_IMAGE_DOS_HEADER *dosHeader;
    EFI_IMAGE_NT_HEADERS64 *ntHeaders;
    EFI_IMAGE_SECTION_HEADER *sections;
    UINT16 i;
    
    dosHeader = (EFI_IMAGE_DOS_HEADER *)imageBase;
    ntHeaders = (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)imageBase + dosHeader->e_lfanew);
    
    sections = (EFI_IMAGE_SECTION_HEADER *)((UINT8 *)&ntHeaders->OptionalHeader + 
                                          ntHeaders->FileHeader.SizeOfOptionalHeader);
    
    for (i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        BOOLEAN isTextSection = TRUE;
        const char textName[] = ".text";
        
        for (UINTN j = 0; j < 6; j++) {
            if (sections[i].Name[j] != textName[j]) {
                isTextSection = FALSE;
                break;
            }
        }

        if (isTextSection) {
            UINT32 alignedSize = (sections[i].Misc.VirtualSize + ntHeaders->OptionalHeader.SectionAlignment - 1) 
                                & ~(ntHeaders->OptionalHeader.SectionAlignment - 1);
            
            UINT32 freeSpace = alignedSize - sections[i].Misc.VirtualSize;
            
            if (freeSpace >= 14) {
                UINT32 rva = sections[i].VirtualAddress + sections[i].Misc.VirtualSize;
                *outRva = rva;
                return (UINT8 *)imageBase + rva;
            }
        }
    }

    return NULL;
}

static EFI_STATUS PatchExportTable(VOID *imageBase, const char *exportName, UINT32 newRva) {
    EFI_IMAGE_DOS_HEADER *dosHeader;
    EFI_IMAGE_NT_HEADERS64 *ntHeaders;
    EFI_IMAGE_EXPORT_DIRECTORY *exportDir;
    UINT32 *nameTable;
    UINT16 *ordinalTable;
    UINT32 *functionTable;
    UINT32 exportDirRva;
    UINT32 i;
    
    dosHeader = (EFI_IMAGE_DOS_HEADER *)imageBase;
    ntHeaders = (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)imageBase + dosHeader->e_lfanew);
    
    exportDirRva = ntHeaders->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportDirRva == 0) {
        return EFI_NOT_FOUND;
    }

    exportDir = (EFI_IMAGE_EXPORT_DIRECTORY *)((UINT8 *)imageBase + exportDirRva);
    nameTable = (UINT32 *)((UINT8 *)imageBase + exportDir->AddressOfNames);
    ordinalTable = (UINT16 *)((UINT8 *)imageBase + exportDir->AddressOfNameOrdinals);
    functionTable = (UINT32 *)((UINT8 *)imageBase + exportDir->AddressOfFunctions);

    for (i = 0; i < exportDir->NumberOfNames; i++) {
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
            functionTable[ordinal] = newRva;
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}
