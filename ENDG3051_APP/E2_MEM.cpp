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
E2_MEM::E2_MEM(uint8_t _port_addr)
{
  this->IIC_E2_ADDR = 0xA0 | _port_addr << 1; // control bit + port address
  Kernel::OS.IICDriver.IICWrite(IIC_E2_ADDR, 0x0, 1); // write address
}

/*\ ---------------------------------------------
|*| @name: sizeof_cchp
|*| @description:
\*/
uint8_t E2_MEM::sizeof_cchp(const char *_pch)
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
  struct page_rw
  {
    unsigned char addr[2];
    unsigned char data[128];
  } page_write;

  auto _data_size = sizeof_cchp(_data);
  int _it = 0;

  do
  {
    page_write.addr = {(_addr >> 8) & 0xFF, _addr & 0x0FF};

    for (int i = 0; i < sizeof(page_write.data); i++)
      page_write.data[i] = _data[i + (sizeof(page_write.data) * _it)];

    Kernel::OS.IICDriver.IICWrite(IIC_E2_ADDR, (unsigned char *)&page_write, sizeof(page_write.data) + 2);

    _data_size -= sizeof(page_write.data);
    _addr += sizeof(page_write.data);
    _it++;

  } while (_data_size > 0);

  return 0;
}

/*\ ---------------------------------------------
|*| @name: Read
|*| @description:
\*/
int E2_MEM::Read(uint16_t _addr){
  return 0;
}
