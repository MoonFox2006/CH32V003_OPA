#pragma once

#include <stdint.h>
#include <stdbool.h>

enum : uint8_t { BIN = 2, OCT = 8, DEC = 10, HEX = 16 };

class Print {
public:
  virtual uint16_t write(char c) = 0;
  virtual uint16_t write(const char *buf, uint16_t count);

  uint16_t print(char c) {
    return write(c);
  }
  uint16_t print(const char *str);
  uint16_t print(uint32_t val, uint8_t base = DEC);
  uint16_t print(int32_t val, uint8_t base = DEC);
  uint16_t print(double val, uint8_t prec = 2);
  uint16_t println() {
    return print(CRLF);
  }
  uint16_t println(char c);
  uint16_t println(const char *str);
  uint16_t println(uint32_t val, uint8_t base = DEC);
  uint16_t println(int32_t val, uint8_t base = DEC);
  uint16_t println(double val, uint8_t prec = 2);

  virtual void flush() {}

protected:
  static const char CRLF[];
};
