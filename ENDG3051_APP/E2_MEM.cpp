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
E2_MEM::E2_MEM(uint8_t _port_addr, uint8_t _mrsB)
{
  this->_IIC_E2_ADDR_ = 0xA0 | _port_addr << 1; // control bit + port address
  this->_max_rec_size_ = _mrsB;
  Kernel::OS.IICDriver.IICWrite(_IIC_E2_ADDR_, 0x0, 1); // write address
}

/*\ ---------------------------------------------
|*| @name: sizeof_cchp
|*| @description:
\*/
uint8_t E2_MEM::sizeof_cchp(const cahr *_pch)
{
  uint16_t _it = 0;
  while (_pch[_it] != '\0')
    _it++;
  return _it;
}

/*\ ---------------------------------------------
|*| @name: Write
|*| @description:
\*/
int E2_MEM::Write(uint16_t _addr, const char *_data)
{
  page_rw
  {
    unsigned char addr[2];
    unsigned char data[this->max_rec_size_B];
  }
  write;

  auto _data_size = sizeof_cchp(_data);
  int _it = 0;

  // check if address is empty, then write --> output address location
  //  else : skip to empty address --> output address location

  do
  {
    write.addr[0] = (_addr >> 8) & 0xFF;
    write.addr[1] = _addr & 0x0FF;

    for (int i = 0; i < this->max_rec_size_B; i++)
      write.data[i] = _data[i + (this->max_rec_size_B * _it)];

    Kernel::OS.IICDriver.IICWrite(_IIC_E2_ADDR_, (unsigned char *)&write, this->max_rec_size_B + 2);

    _data_size -= max_rec_size_B;
    _addr++;
    _it++;
  } while (_data_size > 0);

  return Serial.write(write.data, _data_size) // temp;
}

/*\ ---------------------------------------------
|*| @name: Read
|*| @description:
\*/
int E2_MEM::Read(uint16_t _addr, uint8_t _data_size)
{

  // unsigned char addr[2] = { (_address>>8)& 0xff, _address & 0xff };
  // unsigned char databuffer[400];
  // int datalen = 300;
  // Kernel::OS.IICDriver.IICWrite(IIC_CTRb_ADDR_E2, addr, 2)
  // Kernel::OS.IICDriver.IICRead(IIC_CTRb_ADDR_E2, databuffer, datalen)
  return 0;
}

int E2_MEM::Read(struct page_rw *_read) {}