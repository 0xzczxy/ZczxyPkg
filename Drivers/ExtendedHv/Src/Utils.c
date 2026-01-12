#include "ExtendedHv.h"
#include "WinDefines.h"

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);

// Public Functions
UINT64 FindPatternImage(IN VOID* imageBase, IN CONST CHAR8* pattern);
UINT64 FindPattern(IN VOID* base, IN UINTN size, IN CONST CHAR8* pattern);

// Private Functions
static BOOLEAN MatchPattern(IN CONST UINT8* data, IN CONST CHAR8* pattern, IN UINTN patternLen);
static UINTN GetPatternLength(IN CONST CHAR8* pattern);

// Implementation

UINT64 FindPatternImage(IN VOID* imageBase, IN CONST CHAR8* pattern) {
  if (!imageBase || !pattern) return 0;
  
  PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageBase;
  if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
    SerialPrint("[!] Invalid PE image\n");
    return 0;
  }
  
  PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)((UINT64)imageBase + dosHeader->e_lfanew);
  if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
    SerialPrint("[!] Invalid NT signature\n");
    return 0;
  }
  
  // Scan only executable sections
  PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
  
  for (UINT16 i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
    // Only scan executable sections
    if (!(section[i].Characteristics & EFI_IMAGE_SCN_MEM_EXECUTE)) {
      continue;
    }
    
    UINT64 sectionBase = (UINT64)imageBase + section[i].VirtualAddress;
    UINTN sectionSize = section[i].Misc.VirtualSize;
    
    SerialPrint("[*] Scanning section %c%c%c%c%c%c%c%c (0x%p, size: %u)\n",
      section[i].Name[0], section[i].Name[1], section[i].Name[2], section[i].Name[3],
      section[i].Name[4], section[i].Name[5], section[i].Name[6], section[i].Name[7],
      sectionBase, sectionSize
    );
    
    UINT64 result = FindPattern((VOID*)sectionBase, sectionSize, pattern);
    if (result) {
      SerialPrint("[+] Pattern found at 0x%p\n", result);
      return result;
    }
  }
  
  SerialPrint("[!] Pattern not found\n");
  return 0;
}

UINT64 FindPattern(IN VOID* base, IN UINTN size, IN CONST CHAR8* pattern) {
  if (!base || !pattern || size == 0) return 0;
  
  UINTN patternLen = GetPatternLength(pattern);
  if (patternLen == 0) return 0;
  
  UINT8* data = (UINT8*)base;
  
  for (UINTN i = 0; i < size - patternLen; i++) {
    if (MatchPattern(&data[i], pattern, patternLen)) {
      return (UINT64)(&data[i]);
    }
  }
  
  return 0;
}

static BOOLEAN MatchPattern(IN CONST UINT8* data, IN CONST CHAR8* pattern, IN UINTN patternLen) {
  for (UINTN i = 0; i < patternLen; i++) {
    // Skip whitespace in pattern
    while (pattern[0] == ' ') pattern++;
    
    // Wildcard - matches any byte
    if (pattern[0] == '?') {
      pattern++;
      continue;
    }
    
    // Parse hex byte
    UINT8 high = 0, low = 0;
    
    if (pattern[0] >= '0' && pattern[0] <= '9') {
      high = pattern[0] - '0';
    } else if (pattern[0] >= 'A' && pattern[0] <= 'F') {
      high = pattern[0] - 'A' + 10;
    } else if (pattern[0] >= 'a' && pattern[0] <= 'f') {
      high = pattern[0] - 'a' + 10;
    } else {
      return FALSE;
    }
    pattern++;
    
    if (pattern[0] >= '0' && pattern[0] <= '9') {
      low = pattern[0] - '0';
    } else if (pattern[0] >= 'A' && pattern[0] <= 'F') {
      low = pattern[0] - 'A' + 10;
    } else if (pattern[0] >= 'a' && pattern[0] <= 'f') {
      low = pattern[0] - 'a' + 10;
    } else {
      return FALSE;
    }
    pattern++;
    
    UINT8 expectedByte = (high << 4) | low;
    if (data[i] != expectedByte) {
      return FALSE;
    }
  }
  
  return TRUE;
}

static UINTN GetPatternLength(IN CONST CHAR8* pattern) {
  UINTN count = 0;
  
  while (*pattern) {
    // Skip whitespace
    while (*pattern == ' ') pattern++;
    if (!*pattern) break;
    
    // Check for wildcard or hex digit
    if (*pattern == '?') {
      count++;
      pattern++;
    } else if ((*pattern >= '0' && *pattern <= '9') ||
               (*pattern >= 'A' && *pattern <= 'F') ||
               (*pattern >= 'a' && *pattern <= 'f')) {
      // Expect two hex digits
      pattern += 2;
      count++;
    } else {
      return 0; // Invalid pattern
    }
  }
  
  return count;
}
