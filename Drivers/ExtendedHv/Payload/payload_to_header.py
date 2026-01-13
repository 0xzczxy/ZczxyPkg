#!/usr/bin/env python3
"""
Converts payload.bin to a C header file containing the binary as a byte array.

This script reads the raw binary payload and generates a header file that can be
included in the main driver. The header includes:
  - The raw binary data as a byte array
  - Metadata about offsets and sizes
  - Useful macros for accessing the payload components

Usage:
    python3 payload_to_header.py [input_binary] [output_header]

Example:
    python3 payload_to_header.py payload.bin payload_data.h
"""

import sys
import os
from pathlib import Path


def generate_header(binary_data: bytes, binary_filename: str) -> str:
    """
    Generate a C header file from binary data.
    
    Args:
        binary_data: The raw binary bytes
        binary_filename: Original filename (for documentation)
    
    Returns:
        String containing the complete C header file
    """
    
    size = len(binary_data)
    
    # Create the header guard
    guard_name = "PAYLOAD_DATA_H"
    
    # Format binary as hex array with nice columns
    hex_lines = []
    bytes_per_line = 16
    
    for i in range(0, len(binary_data), bytes_per_line):
        chunk = binary_data[i:i + bytes_per_line]
        hex_bytes = ", ".join(f"0x{b:02x}" for b in chunk)
        hex_lines.append(f"    {hex_bytes},")
    
    # Remove trailing comma from last line
    if hex_lines:
        hex_lines[-1] = hex_lines[-1].rstrip(",")
        hex_lines[-1] = hex_lines[-1] + ","
    
    hex_array = "\n".join(hex_lines)
    
    header = f"""\
#ifndef {guard_name}
#define {guard_name}

#include <stdint.h>
#include <stddef.h>

/*
 * ExtendedHV Payload Binary
 * 
 * Generated from: {binary_filename}
 * Binary size: {size} bytes (0x{size:x})
 * 
 * Layout:
 *   Offset 0x00-0x07: G_original_offset_from_hook (int64_t)
 *   Offset 0x08-0x0A: G_arch (int32_t)
 *   Offset 0x10+:     hooked_vmexit_handler function
 * 
 * The driver will:
 *   1. Place this payload in hypervisor memory
 *   2. Patch the offset 0x00 value with the relative offset to original handler
 *   3. Patch the call instruction to jump to offset 0x10
 */

/* Payload binary data */
static const uint8_t g_payload_data[] = {{
{hex_array}
}};

/* Payload metadata */
#define PAYLOAD_SIZE                    {size}
#define PAYLOAD_GLOBAL_OFFSET           0x00
#define PAYLOAD_ARCH_OFFSET             0x08
#define PAYLOAD_FUNCTION_OFFSET         0x10
#define PAYLOAD_GLOBAL_SIZE             8
#define PAYLOAD_ARCH_SIZE               4

/* Helper macro to get pointer to function within payload */
#define PAYLOAD_FUNCTION_PTR(base) \\
    ((uint64_t (__attribute__((ms_abi)) *)(void*, void*, void*)) \\
     ((uintptr_t)(base) + PAYLOAD_FUNCTION_OFFSET))

 /* Helper macro to get pointer to global offset field */
// WARNING: INT64 and int64_t may be different, in definition (i.e. long long int vs long int) but, they should both be 64 bits making this safe.
#define PAYLOAD_GLOBAL_PTR(base) \\
    ((INT64 *)((uintptr_t)(base) + PAYLOAD_GLOBAL_OFFSET))

/* Helper macro to get pointer to global offset field */
#define PAYLOAD_ARCH_PTR(base) \\
    ((INT32 *)((uintptr_t)(base) + PAYLOAD_ARCH_OFFSET))



#endif /* {guard_name} */
"""
    
    return header


def main():
    """Main entry point."""
    
    # Default filenames
    input_file = "payload.bin"
    output_file = "payload_data.h"
    
    # Parse command line arguments
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    if len(sys.argv) > 2:
        output_file = sys.argv[2]
    
    # Verify input file exists
    if not os.path.isfile(input_file):
        print(f"Error: Input file '{input_file}' not found", file=sys.stderr)
        print(f"Usage: {sys.argv[0]} [input_binary] [output_header]", file=sys.stderr)
        sys.exit(1)
    
    # Read binary file
    with open(input_file, "rb") as f:
        binary_data = f.read()
    
    print(f"✓ Read {len(binary_data)} bytes from '{input_file}'")
    
    # Generate header
    header_content = generate_header(binary_data, os.path.basename(input_file))
    
    # Write output file
    with open(output_file, "w") as f:
        f.write(header_content)
    
    print(f"✓ Generated '{output_file}'")
    print(f"  - Size: {len(binary_data)} bytes")
    print(f"  - Global offset field: 0x00 (8 bytes)")
    print(f"  - Arch offset field: 0x08 (4 bytes)")
    print(f"  - Function starts at: 0x10")
    
    return 0


if __name__ == "__main__":
    sys.exit(main())
