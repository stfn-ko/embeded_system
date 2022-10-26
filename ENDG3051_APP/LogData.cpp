/*\ ---------------------------------------------
|*| @name: LogData.cpp
|*| @INFO: FOR CONTEXT CHECK OUT INCLUDE FILES
\*/

#include "LogData.h"

/*\ ---------------------------------------------
|*| @name: RWMem
|*| @description: parameterized constructor that takes address of E2 device from the board;
|*| @PARAM: _port_addr describes the type of wiring and ranges from 0-7
\*/
LogData::LogData(uint8_t _port_addr)
{
  this->IIC_ADDR_E2 = 0xA0 | _port_addr << 1;                  // control bit + port address
  Kernel::OS.IICDriver.IICWrite(IIC_ADDR_E2, E2_BASE_ADDR, 1); // write address
}

/*\ ---------------------------------------------
|*| @name: sizeof_cchp
|*| @description:
\*/
uint8_t LogData::sizeof_cchp(const char *_pch)
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
int LogData::Write(uint16_t _addr, const char *_data)
{
  struct page_rw
  {
    unsigned char addr[2];
    unsigned char data[128];
  } page_write;

  auto _data_size = sizeof_cchp(_data);
  int _it = 0;

  // add hash lookup table for (start address, end address, element's number, length of data in bytes)

  do
  {
    page_write.addr = {TO_MSB_LSB_FROM(_addr)};

    for (int i = 0; i < sizeof(page_write.data); i++)
      page_write.data[i] = _data[i + (sizeof(page_write.data) * _it)];

    Kernel::OS.IICDriver.IICWrite(IIC_ADDR_E2, (unsigned char *)&page_write, sizeof(page_write));

    _data_size -= sizeof(page_write.data);
    _addr += sizeof(page_write.data) + 1;

    if (_addr > E2_END_ADDR)
      _addr = E2_BASE_ADDR;

    _it++;

  } while (_data_size > 0);

  return 0;
}

/*\ ---------------------------------------------
|*| @name: Read
|*| @description:
\*/
int LogData::Read(uint16_t _addr)
{
  unsigned char addr[2] = TO_MSB_LSB_FROM(_addr);
  unsigned char databuffer[128];

  Kernel::OS.IICDriver.IICWrite(IIC_ADDR_E2, addr, 2);
  Kernel::OS.IICDriver.IICRead(IIC_ADDR_E2, databuffer, sizeof(databuffer));
  return 0;
}
