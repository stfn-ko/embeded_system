/*\ ---------------------------------------------
|*| @name: LogData.H
|*| @author: Stephan Kolontay 2022
|*| @date: 18/10/2022
|*| @description: Derived from Task - this uses a non-volatile E2 data logger to 
|*| maintain a recording of events that have taken place
|*| @Note: max size: 512 pages (655536B | 128B/page), I want each writeto be 32B long (2044 entries + 4 reserved for meta data) 0-2044
|*| @Note: data entry exapmle: "dd/mm/yy,hh:mm:ss,tt,aaa,ddd\n"
\*/ 

#ifndef LOGDATA_H_
#define LOGDATA_H_

#include "kernel.h"

#define E2_END_ADDR 0xFFFF
#define DEVLOCK 312

typedef const char c_char;
typedef unsigned char u_char;

class LogData { 
  uint8_t IIC_ADDR_E2; //device address
  const uint16_t L_ADDR = 0xFF80; //2044 // 65408
  bool ok(int _a, int _d);

  //disabeled
  int WriteAuto(c_char* _data); 
  int get_addr(); // get last recently used address
  void upd_addr(int _caddr); // update last recently used address
  void Reset(int _dl = 0);
	
	public:
	LogData(uint8_t _port_addr = 0); //init slave eeprom's address //doesnt have unique value check (may crush on the bus);
  ~LogData(){};
  
  int Write(int _page, c_char* _data, int _dl = 0); //_dl is devlock for unlimited access to the eeprom
  int ReadAll(); // reads all pages at once;
  int ReadPage(int _pg, int _dl = 0); // reads specific page
  int ReadFT(int _saddr, int _eaddr, int _dl = 0); // Reads from-to pages
};

#endif
