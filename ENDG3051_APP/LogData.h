/*\ ---------------------------------------------
|*| @name: LogData.h
|*| @author: Stephan Kolontay 2022
|*| @date: 18/10/2022
|*| @description: Derived from Task - this uses a non-volatile E2 data logger to
|*| maintain a recording of events that have taken place
|*| @Note: max size: 500 pages (64000b | 128B/page)
\*/

#ifndef LOGDATA_H
#define LOGDATA_H

#include "kernel.h"

#define E2_BASE_ADDR 0x0
#define E2_END_ADDR 0xFA00

#define TO_MSB_LSB_FROM(n) (n >> 8) & 0xFF, n & 0x0FF

class LogData
{
  uint8_t IIC_ADDR_E2;
  uint8_t sizeof_cchp(const char *_pch);

public:
  LogData(uint8_t _port_addr = 0); // init slave eeprom's address //doesnt have unique value check (may crush on the bus);
  ~LogData(){};

  int Write(uint16_t _addr, const char *_data);

  int Read(uint16_t _addr);
};

#endif // LOGDATA_H
