//
// Enhanced serial debugging functions for hypervisor payload
// WARNING: No initialization of the serial port happens since the driver handles initialization
// 

#include <stdint.h>

// Imports
// None

// Public Globals
// None

// Public Functions
void serial_write(const char *string);
void serial_write_pointer(const char *msg, void *addr);
void serial_write_hex32(const char *msg, uint32_t value);
void serial_write_hex64(const char *msg, uint64_t value);
void serial_write_decimal(const char *msg, int64_t value);
void serial_write_hex_dump(const char *msg, void *addr, uint32_t length);
void serial_write_hex_line(uint32_t offset, void *data, uint32_t count);

// Private Globals
static const char g_hex_chars[] = "0123456789ABCDEF";

// Private Functions
static void serial_write_char(char cin);
static inline void IoWrite8(uint16_t port, uint8_t value);
static inline uint8_t IoRead8(uint16_t port);
static void int64_to_decimal(int64_t value, char *buffer);

// Implementation

void serial_write_hex32(const char *msg, uint32_t value) {
  char buffer[64];
  int pos = 0;
  
  // Copy message
  while (msg && msg[pos] && pos < 40) {
    buffer[pos] = msg[pos];
    pos++;
  }
  
  // Add ": 0x"
  buffer[pos++] = ':';
  buffer[pos++] = ' ';
  buffer[pos++] = '0';
  buffer[pos++] = 'x';
  
  // Convert to hex (8 digits)
  for (int i = 28; i >= 0; i -= 4) {
    buffer[pos++] = g_hex_chars[(value >> i) & 0xF];
  }
  
  buffer[pos++] = '\n';
  buffer[pos] = '\0';
  
  serial_write(buffer);
}

void serial_write_hex64(const char *msg, uint64_t value) {
  char buffer[64];
  int pos = 0;
  
  // Copy message
  while (msg && msg[pos] && pos < 40) {
    buffer[pos] = msg[pos];
    pos++;
  }
  
  // Add ": 0x"
  buffer[pos++] = ':';
  buffer[pos++] = ' ';
  buffer[pos++] = '0';
  buffer[pos++] = 'x';
  
  // Convert to hex (16 digits)
  for (int i = 60; i >= 0; i -= 4) {
    buffer[pos++] = g_hex_chars[(value >> i) & 0xF];
  }
  
  buffer[pos++] = '\n';
  buffer[pos] = '\0';
  
  serial_write(buffer);
}

void serial_write_decimal(const char *msg, int64_t value) {
  char buffer[96];
  char num_buf[32];
  int pos = 0;
  
  // Copy message
  while (msg && msg[pos] && pos < 40) {
    buffer[pos] = msg[pos];
    pos++;
  }
  
  // Add ": "
  buffer[pos++] = ':';
  buffer[pos++] = ' ';
  
  // Convert to decimal
  int64_to_decimal(value, num_buf);
  
  // Copy number to buffer
  int i = 0;
  while (num_buf[i]) {
    buffer[pos++] = num_buf[i++];
  }
  
  buffer[pos++] = '\n';
  buffer[pos] = '\0';
  
  serial_write(buffer);
}

void serial_write_hex_dump(const char *msg, void *addr, uint32_t length) {
  uint8_t *data = (uint8_t *)addr;
  
  if (msg) {
    serial_write(msg);
    serial_write("\n");
  }
  
  // Dump in 16-byte lines
  for (uint32_t offset = 0; offset < length; offset += 16) {
    uint32_t line_len = length - offset;
    if (line_len > 16) {
      line_len = 16;
    }
    
    serial_write_hex_line(offset, &data[offset], line_len);
  }
}

void serial_write_hex_line(uint32_t offset, void *data, uint32_t count) {
  char buffer[128];
  uint8_t *bytes = (uint8_t *)data;
  int pos = 0;
  
  // Print offset (8 hex digits)
  for (int i = 28; i >= 0; i -= 4) {
    buffer[pos++] = g_hex_chars[(offset >> i) & 0xF];
  }
  
  buffer[pos++] = ':';
  buffer[pos++] = ' ';
  
  // Print hex bytes (up to 16)
  for (uint32_t i = 0; i < 16; i++) {
    if (i < count) {
      buffer[pos++] = g_hex_chars[(bytes[i] >> 4) & 0xF];
      buffer[pos++] = g_hex_chars[bytes[i] & 0xF];
    } else {
      buffer[pos++] = ' ';
      buffer[pos++] = ' ';
    }
    buffer[pos++] = ' ';
    
    // Extra space after 8 bytes
    if (i == 7) {
      buffer[pos++] = ' ';
    }
  }
  
  buffer[pos++] = ' ';
  buffer[pos++] = '|';
  
  // Print ASCII representation
  for (uint32_t i = 0; i < count; i++) {
    char c = bytes[i];
    // Print printable characters, '.' for non-printable
    if (c >= 32 && c <= 126) {
      buffer[pos++] = c;
    } else {
      buffer[pos++] = '.';
    }
  }
  
  buffer[pos++] = '|';
  buffer[pos++] = '\n';
  buffer[pos] = '\0';
  
  serial_write(buffer);
}

void serial_write_pointer(const char *msg, void *addr) {
  serial_write_hex64(msg, (uint64_t)addr);
}

void serial_write(const char *string) {
  if (!string) {
    return;
  }

  while (*string) {
    // Convert LF to CRLF for proper terminal display
    if (*string == '\n') {
      serial_write_char('\r');
    }
    serial_write_char(*string++);
  }
}

static void serial_write_char(char cin) {
  uint16_t port_base = 0x3f8;

  // Wait for transmit buffer to be ready
  while ((IoRead8(port_base + 5) & 0x20) == 0);

  // Write Character
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

// Helper function to convert int64 to decimal string
static void int64_to_decimal(int64_t value, char *buffer) {
  int pos = 0;
  uint64_t abs_value;
  
  // Handle negative numbers
  if (value < 0) {
    buffer[pos++] = '-';
    abs_value = (uint64_t)(-value);
  } else {
    abs_value = (uint64_t)value;
  }
  
  // Handle zero
  if (abs_value == 0) {
    buffer[pos++] = '0';
    buffer[pos] = '\0';
    return;
  }
  
  // Convert digits in reverse
  char temp[32];
  int temp_pos = 0;
  while (abs_value > 0) {
    temp[temp_pos++] = '0' + (abs_value % 10);
    abs_value /= 10;
  }
  
  // Reverse into output buffer
  for (int i = temp_pos - 1; i >= 0; i--) {
    buffer[pos++] = temp[i];
  }
  
  buffer[pos] = '\0';
}
