#ifndef __EXTENDEDHV_H
#define __EXTENDEDHV_H

//
// Uefi Includes
// 
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/LoadedImage.h>
#include <IndustryStandard/PeImage.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePath.h>

// 
// Allignment Macros
// 
#define ALIGN_DOWN(Value, Alignment) \
  ((Value) & ~((Alignment) - 1))

#define ALIGN_UP(Value, Alignment) \
  (((Value) + (Alignment) - 1) & ~((Alignment) - 1))

//
// Patch type
// 
struct patchinfo {
  VOID *original_function;   // Pointer to the original function
  VOID *target_function;     // Pointer to the hook function  
  unsigned char buffer[14];  // Backup of original bytes
};
typedef struct patchinfo patchinfo_t;

#endif // __EXTENDEDHV_H
