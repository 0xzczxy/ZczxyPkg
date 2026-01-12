#include <Library/PrintLib.h>
#include <Library/IoLib.h>
#include <Library/DevicePathLib.h>

//
// Serial port registers
//
#define SERIAL_DATA_REG     (serial_port_base + 0)
#define SERIAL_INT_EN_REG   (serial_port_base + 1)
#define SERIAL_FIFO_REG     (serial_port_base + 2)
#define SERIAL_LINE_CTL_REG (serial_port_base + 3)
#define SERIAL_MODEM_CTL_REG (serial_port_base + 4)
#define SERIAL_LINE_STS_REG (serial_port_base + 5)

//
// Line status register bits
//
#define LSR_TXRDY           0x20

// Import
// None

// Public Globals
UINT16 COM1 = 0x3F8;
UINT16 COM2 = 0x2F8;
UINT16 COM3 = 0x3E8;

// Public Functions
VOID EFIAPI SerialPortInitialize(UINT16);
VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
VOID EFIAPI SerialPrintUnicode(IN CONST CHAR8 *label, IN CONST CHAR16 *string);

// Private Globals
static UINT16 serial_port_base = 0;

// Private Functions
static VOID EFIAPI SerialPortWrite(IN CHAR8 cin);
static VOID EFIAPI SerialPortWriteString(IN CONST CHAR8 *string);



VOID EFIAPI SerialPortInitialize(UINT16 base) {
  serial_port_base = base;
  
  //
  // Set baud rate (115200)
  //
  IoWrite8(SERIAL_LINE_CTL_REG, 0x80);  // Enable DLAB
  IoWrite8(SERIAL_DATA_REG, 0x01);       // Divisor low byte
  IoWrite8(SERIAL_INT_EN_REG, 0x00);     // Divisor high byte

  //
  // Configure: 8 data bits, 1 stop bit, no parity
  //
  IoWrite8(SERIAL_LINE_CTL_REG, 0x03);

  //
  // Enable FIFO
  //
  IoWrite8(SERIAL_FIFO_REG, 0xC7);

  //
  // Assert RTS and DTR
  //
  IoWrite8(SERIAL_MODEM_CTL_REG, 0x03);
}

VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...) {
  VA_LIST args;
  CHAR8 buffer[512];
  UINTN length;

  VA_START(args, format);
  length = AsciiVSPrint(buffer, sizeof(buffer), format, args);
  VA_END(args);

  if (length > 0) {
    SerialPortWriteString(buffer);
  }
}

VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value) {
  SerialPrint("%a: 0x%016lx\n", label, value);
}

VOID EFIAPI SerialPrintUnicode(IN CONST CHAR8 *label, IN CONST CHAR16 *string) {
  CHAR8 asciiBuffer[256];
  UINTN i;

  if (!string) {
    SerialPrint("%a: (null)\n", label);
    return;
  }

  //
  // Simple Unicode to ASCII conversion
  //
  for (i = 0; i < 255 && string[i] != 0; i++) {
    asciiBuffer[i] = (CHAR8)(string[i] < 128 ? string[i] : '?');
  }
  asciiBuffer[i] = 0;

  SerialPrint("%a: %a\n", label, asciiBuffer);
}

static VOID EFIAPI SerialPortWrite(IN CHAR8 cin) {
  //
  // Wait for transmit buffer to be ready
  //
  while ((IoRead8(SERIAL_LINE_STS_REG) & LSR_TXRDY) == 0);

  //
  // Write character
  //
  IoWrite8(SERIAL_DATA_REG, cin);
}

static VOID EFIAPI SerialPortWriteString(IN CONST CHAR8 *string) {
  if (!string) {
    return;
  }

  while (*string) {
    //
    // Convert LF to CRLF for proper terminal display
    //
    if (*string == '\n') {
      SerialPortWrite('\r');
    }
    SerialPortWrite(*string++);
  }
}

