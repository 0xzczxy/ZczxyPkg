//
// WARNING: No initialization of the serial port happens since the driver handles initilaization,
//          ensure you match the com port inside here to what you chose in the driver.
// 

#include <stdint.h>

// Imports
// None

// Public Globals
// None

// Public Functions
void serial_write(const char *string);

// Private Globals
// None

// Private Functions
static void serial_write_char(char cin);
static inline void IoWrite8(uint16_t port, uint8_t value);
static inline uint8_t IoRead8(uint16_t port);

// Implementation

void serial_write(const char *string) {
  if (!string) {
    return;
  }

  while (*string) {
    //
    // Convert LF to CRLF for proper terminal display
    //
    if (*string == '\n') {
      serial_write_char('\r');
    }
    serial_write_char(*string++);
  }
}

static void serial_write_char(char cin) {
  uint16_t port_base = 0x3f8;

  //
  // Wait for transmit buffer to be ready
  // 
  while ((IoRead8(port_base + 5) & 0x20) == 0);

  //
  // Write Character
  // 
  IoWrite8(port_base, cin);
}

static inline uint8_t IoRead8(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "dN"(port));
    return value;
}

static inline void IoWrite8(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "dN"(port));
}

