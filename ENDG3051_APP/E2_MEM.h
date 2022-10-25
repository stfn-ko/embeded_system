/*\ ---------------------------------------------
|*| @name: E2_MEM.H
|*| @author: Stephan Kolontay 2022
|*| @date: 18/10/2022
|*| @description: Derived from Task - this uses a non-volatile E2 data logger to
|*| maintain a recording of events that have taken place
|*| @Note: max size: 500 pages (64000b | 128B/page)
\*/

#ifndef E2_MEM_H_
#define E2_MEM_H_

#include "kernel.h"

#define e2_base_addr 0x0
#define e2_end_addr 0xFA00

class E2_MEM
{
  uint8_t IIC_E2_ADDR;
  uint8_t sizeof_cchp(const char *_pch);

public:
  E2_MEM(uint8_t _port_addr = 0); // init slave eeprom's address //doesnt have unique value check (may crush on the bus);
  ~E2_MEM(){};

  int Write(uint16_t _addr, const char *_data);

  int Read(uint16_t _addr);
};

#endif
