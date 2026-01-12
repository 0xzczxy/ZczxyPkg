#include "ExtendedHv.h"
#include "WinDefines.h"

// Imports
// None

// Public Globals
// None

// Public Functions
UINT64 PeGetExport(IN CONST VOID *base, IN CONST CHAR8 *export);

// Private Globals
// None

// Private Functions
// None


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



