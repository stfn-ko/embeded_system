/*\ ---------------------------------------------
|*| @name: LogData.h
|*| @author: Stephan Kolontay 2022
|*| @date: 18/10/2022
|*| @description: Derived from Task - this uses a non-volatile E2 data logger to
|*| maintain a recording of events that have taken place
|*| @Note: max size: 512 pages (65536B | 128B/page), I want each log to be exactly 32B long (1999 entries + 1 reserved for address count)
|*| @Note: data entry exapmle: "dd/mm/yy,hh:mm:ss,tt,aaa,ddd\n"
\*/

#ifndef LOGDATA_H
#define LOGDATA_H

#include "kernel.h"

typedef const char c_char;
typedef unsigned char u_char;

#define E2_END_ADDR 0xFFFF

class LogData
{
  uint8_t IIC_ADDR_E2;
  const uint16_t L_ADDR = 0xFF80;

  int get_addr();
  void upd_addr();

public:
  LogData(uint8_t _port_addr = 0);
  ~LogData(){};

  int Write_32B(c_char *_data);
  int Write_128B(int _pg, c_char *_data, int _devlock = 0);

  int ReadPage(int _pg);
  int ReadFT(int _saddr, int _eaddr, int _pagesize = 128, int _devlock = 0);
  int ReadAll();
};

#endif // LOGDATA_H
