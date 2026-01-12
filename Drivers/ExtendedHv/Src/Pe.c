#include "ExtendedHv.h"
#include "WinDefines.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);

// Public Functions
UINT64 PeGetExport(IN CONST VOID *base, IN CONST CHAR8 *export);
UINT64 PeAddSection(IN UINT64 imageBase, IN CONST CHAR8* sectionName, IN UINT32 virtualSize, IN UINT32 characteristics);

// Implementation

UINT64 PeGetExport(IN CONST VOID *base, IN CONST CHAR8 *export) {
  PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)base;
  if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return 0;

  PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)((UINT64)base + dosHeader->e_lfanew);

  UINT32 exportsRva = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
  if (!exportsRva) return 0;

  PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((UINT64)base + exportsRva);
  UINT32* nameRva = (UINT32*)((UINT64)base + exports->AddressOfNames);

  for (UINT32 i = 0; i < exports->NumberOfNames; ++i) {
    CHAR8* func = (CHAR8*)((UINT64)base + nameRva[i]);
    if (AsciiStrCmp(func, export) == 0) {
      UINT32* funcRva = (UINT32*)((UINT64)base + exports->AddressOfFunctions);
      UINT16* ordinalRva = (UINT16*)((UINT64)base + exports->AddressOfNameOrdinals);

      return (UINT64)base + funcRva[ordinalRva[i]];
    }
  }

  return 0;
}

UINT64 PeAddSection(IN UINT64 imageBase, IN CONST CHAR8* sectionName, IN UINT32 virtualSize, IN UINT32 characteristics) {
  PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageBase;
  if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
    SerialPrint("[!] Invalid DOS signature\n");
    return 0;
  }
  
  PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)(imageBase + dosHeader->e_lfanew);
  if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
    SerialPrint("[!] Invalid NT signature\n");
    return 0;
  }
  
  PIMAGE_FILE_HEADER fileHeader = &ntHeaders->FileHeader;
  UINT16 sizeOfOptionalHeader = fileHeader->SizeOfOptionalHeader;
  UINT32 numberOfSections = fileHeader->NumberOfSections;
  UINT32 sectionAlignment = ntHeaders->OptionalHeader.SectionAlignment;
  UINT32 fileAlignment = ntHeaders->OptionalHeader.FileAlignment;
  
  // Get pointer to first section header
  PIMAGE_SECTION_HEADER firstSection = (PIMAGE_SECTION_HEADER)(
    (UINT64)fileHeader + sizeof(IMAGE_FILE_HEADER) + sizeOfOptionalHeader
  );
  
  // Get pointer to new section header (after last existing section)
  PIMAGE_SECTION_HEADER newSection = &firstSection[numberOfSections];
  PIMAGE_SECTION_HEADER lastSection = &firstSection[numberOfSections - 1];
  
  SerialPrint("[*] Adding section '%a'\n", sectionName);
  SerialPrintHex("  Last section VA", lastSection->VirtualAddress);
  SerialPrintHex("  Last section VSize", lastSection->Misc.VirtualSize);
  
  // Zero out the new section header
  SetMem(newSection, sizeof(IMAGE_SECTION_HEADER), 0);
  
  // Copy section name (max 8 bytes)
  UINTN nameLen = AsciiStrLen(sectionName);
  if (nameLen > IMAGE_SIZEOF_SHORT_NAME) nameLen = IMAGE_SIZEOF_SHORT_NAME;
  CopyMem(newSection->Name, sectionName, nameLen);
  
  // Calculate virtual address (aligned)
  newSection->VirtualAddress = ALIGN_UP(
    lastSection->VirtualAddress + lastSection->Misc.VirtualSize,
    sectionAlignment
  );
  
  // Set sizes
  newSection->Misc.VirtualSize = virtualSize;
  newSection->SizeOfRawData = ALIGN_UP(virtualSize, fileAlignment);
  
  // Set characteristics
  newSection->Characteristics = characteristics;
  
  // Calculate raw data pointer (for disk - not used at runtime but good to set)
  newSection->PointerToRawData = lastSection->PointerToRawData + lastSection->SizeOfRawData;
  
  // Update PE header
  fileHeader->NumberOfSections++;
  ntHeaders->OptionalHeader.SizeOfImage = ALIGN_UP(
    newSection->VirtualAddress + newSection->Misc.VirtualSize,
    sectionAlignment
  );
  
  SerialPrintHex("  New section VA", newSection->VirtualAddress);
  SerialPrintHex("  New section VSize", newSection->Misc.VirtualSize);
  SerialPrintHex("  New SizeOfImage", ntHeaders->OptionalHeader.SizeOfImage);
  SerialPrint("[+] Section added successfully\n");
  
  // Return virtual address of new section
  return imageBase + newSection->VirtualAddress;
}
