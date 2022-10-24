/*\ ---------------------------------------------
|*| @name: RWMEM.H
|*| @Note: 0-write/1-read
|*| @INFO: FOR CONTEXT CHECK OUT INCLUDE FILES
\*/

#include "E2_MEM.h"

/*\ ---------------------------------------------
|*| @name: RWMem
|*| @description: parameterized constructor that takes address of E2 device from the board;
|*| @PARAM: _port_addr describes the type of wiring and ranges from 0-7
\*/
E2_MEM::E2_MEM(uint8_t _port_addr, int _mrs){
  this->_IIC_E2_ADDR_ = 0xA0 | _port_addr << 1; //control bit + port address
  this->_max_rec_size_ = _mrs;
  Kernel::OS.IICDriver.IICWrite(_IIC_E2_ADDR_, 0x0, 1); //write address
}

/*\ ---------------------------------------------
|*| @name: Write
|*| @description: 
\*/
int E2_MEM::Write(uint16_t _addr, const unsigned char* _buffer){
  struct pagewrite{
    unsigned char addr[2];
    unsigned char data[128];
  }wr;
   
  wr.addr[0] = (_addr >> 8) & 0xFF;
  wr.addr[1] =  _addr & 0x0FF;

  for(int i = 0; i < this->_max_rec_size_; i++)
    wr.data[i] = _buffer[i];

   //what if buffer size is more than 128 bytes? what then?
   // maybe : add extra_write() function to append it?
   
  Kernel::OS.IICDriver.IICWrite(_IIC_E2_ADDR_, (unsigned char *)&wr, this->_max_rec_size_ + 2);
}

/*\ ---------------------------------------------
|*| @name: Read
|*| @description: 
\*/
int E2_MEM::Read(uint16_t _addr, unsigned char* _buffer){


  
  //unsigned char addr[2] = { (_address>>8)& 0xff, _address & 0xff };  
  //unsigned char databuffer[400];
  //int datalen = 300;  
  //Kernel::OS.IICDriver.IICWrite(IIC_CTRb_ADDR_E2, addr, 2)
  //Kernel::OS.IICDriver.IICRead(IIC_CTRb_ADDR_E2, databuffer, datalen)
}
