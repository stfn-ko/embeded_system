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

#define E2_base_addr 0x0
#define E2_end_addr 0xFA00

class E2_MEM { 
  
  static uint8_t _IIC_E2_ADDR_;
  static int _max_rec_size_; //max record size in bytes ()
	
	public:
 
	E2_MEM(uint8_t _port_addr = 0, int _mrs = 128); //init slave eeprom's address //doesnt have unique value check (may crush on the bus);

  int Write(uint16_t _addr, const unsigned char* _buffer);
  
  int Read(uint16_t _addr, unsigned char* _buffer);
		
};

#endif