/* 
 * Linker script for ExtendedHV payload
 * 
 * Layout:
 *   - .text.global (G_original_offset_from_hook) - MUST be first
 *   - .text.function (hooked_vmexit_handler) - MUST be immediately after
 *   - Other sections
 */

ENTRY(_start)

SECTIONS
{
  . = 0x0;

  /* 
   * Global data section - contains G_original_offset_from_hook
   * MUST come first so driver knows it's at offset 0x0
   */
  .data.global : ALIGN(8)
  {
    *(.data.global)
  }

  /* 
   * Function section - contains hooked_vmexit_handler
   * Driver will patch call instruction to point here
   * Immediately follows .data.global
   */
  .text.function : ALIGN(8)
  {
    *(.text.function)
  }

  /* All other text sections */
  .text : ALIGN(8)
  {
    *(.text*)
    *(.rodata*)
  }

  /* Data sections */
  .data : ALIGN(8)
  {
    *(.data*)
  }

  /* BSS sections - will be zeroed at runtime */
  .bss : ALIGN(8)
  {
    *(.bss*)
    *(COMMON)
  }

  /* Discard unnecessary sections */
  /DISCARD/ :
  {
    *(.note.*)
    *(.comment)
    *(.eh_frame)
    *(.gnu.hash)
  }
}
