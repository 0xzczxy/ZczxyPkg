#ifndef __PAYLOAD_H
#define __PAYLOAD_H

#include "ExtendedHv.h"

// Payload markers (defined in Payload.c)
extern volatile const UINT64 PayloadStartMarker;
extern volatile const UINT64 PayloadEndMarker;

// Get payload base address
static inline VOID* GetPayloadBase(VOID) {
  return (VOID*)&PayloadStartMarker;
}

// Get payload size
static inline UINTN GetPayloadSize(VOID) {
  UINT64 start = (UINT64)&PayloadStartMarker;
  UINT64 end = (UINT64)&PayloadEndMarker;
  
  // Align to page boundary for safety
  UINTN size = (UINTN)(end - start);
  return ALIGN_UP(size, EFI_PAGE_SIZE);
}

// Payload function prototypes (will be called from hv.exe context)
void PayloadEntry(void);
UINT64 PayloadReadPhysicalMemory(UINT64 physicalAddress, UINT64 size, void* buffer);
UINT64 PayloadWritePhysicalMemory(UINT64 physicalAddress, UINT64 size, const void* buffer);

#endif // __PAYLOAD_H
