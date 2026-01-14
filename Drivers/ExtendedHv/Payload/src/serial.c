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
void serial_write_pointer(const char *msg, void *addr);

// Private Globals
// None

// Private Functions
static void serial_write_char(char cin);
static inline void IoWrite8(uint16_t port, uint8_t value);
static inline uint8_t IoRead8(uint16_t port);

// Implementation

void serial_write_pointer(const char *msg, void *addr) {
  static const char hex_chars[] = "0123456789ABCDEF";
  char buffer[64];
  uint64_t value = (uint64_t)addr;
  int pos = 0;
  int i;
  
  // Copy message
  while (msg[pos] && pos < 40) {
    buffer[pos] = msg[pos];
    pos++;
  }
  
  // Add ": 0x"
  buffer[pos++] = ':';
  buffer[pos++] = ' ';
  buffer[pos++] = '0';
  buffer[pos++] = 'x';
  
  // Convert pointer to hex (16 digits)
  for (i = 60; i >= 0; i -= 4) {
    buffer[pos++] = hex_chars[(value >> i) & 0xF];
  }
  
  buffer[pos++] = '\n';
  buffer[pos] = '\0';
  
  serial_write(buffer);
}


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

