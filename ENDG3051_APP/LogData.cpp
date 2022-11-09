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
LogData::LogData(uint8_t _port_addr) : IIC_ADDR_E2(0xA0 | _port_addr << 1) {}

/*\ ---------------------------------------------
|*| @name: get_addr
|*| @description: get last used address
\*/
int LogData::get_addr()
{
  int dbuff[2];
  u_char addr[2] = {this->L_ADDR >> 8, this->L_ADDR};

  Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, addr, 2);
  Kernel::OS.IICDriver.IICRead(this->IIC_ADDR_E2, (u_char *)&dbuff, 2);

  return (dbuff[0] << 8 | dbuff[1]);
}

/*\ ---------------------------------------------
|*| @name: upd_lru_addr
|*| @description: get last recently used address
\*/
void LogData::upd_addr()
{
  struct load
  {
    u_char addr[2];
    int loc[2];
  } upd;

  int new_addr = get_addr() + 32;

  if (new_addr >= this->L_ADDR)
    new_addr = 0;

  upd.addr[0] = this->L_ADDR >> 8;
  upd.addr[1] = this->L_ADDR;
  upd.loc[2] = new_addr >> 8;
  upd.loc[3] = new_addr;

  Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, (u_char *)&upd, 4);
}

/*\ ---------------------------------------------
|*| @name: Write_32
|*| @description:
\*/
int LogData::Write_32B(c_char *_data)
{
  struct buffer
  {
    u_char addr[2];
    char data[128];
  } wr;

  // gets start address (saddr) of 128B page we are at
  int new_addr = get_addr();
  int it = (new_addr / 32) % 4;
  int saddr = new_addr - new_addr % 128;

  wr.addr[0] = saddr >> 8;
  wr.addr[1] = saddr;

  if (it)
  {
    Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, wr.addr, 2);
    Kernel::OS.IICDriver.IICRead(this->IIC_ADDR_E2, (u_char *)&wr, 32*it));

    // now our dbuff contains data from a 128B page (consists of 4x32B pages)

    wr.addr[0] = new_addr >> 8;
    wr.addr[1] = new_addr;
  }

  for (auto i = 0; i < 32; ++i)
    wr.data[i + 32 * it] = _data[i];

  int _ = Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, (u_char *)&wr, 130);

  if (!_)
    this->upd_addr();

  return _;
}

/*\ ---------------------------------------------
|*| @name: Write_M128
|*| @description:
\*/
int LogData::Write_128B(int _pg, c_char *_data, int _devlock)
{
  if (_pg > 511 && _devlock != 312)
    return 1;

  uint16_t _it = 0;
  while (_data[_it] != '\0')
    _it++;

  if (_it > 128)
    return 1;

  struct buffer
  {
    u_char addr[2];
    char data[128];
  } wr;

  wr.addr[0] = (_pg * 128) >> 8;
  wr.addr[1] = _pg * 128;

  strcpy(wr.data, _data);

  return Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, (u_char *)&wr, _it);
}

/*\ ---------------------------------------------
|*| @name: ReadPage
|*| @description:
\*/
int LogData::ReadPage(int _pg)
{
  if (_pg > 2044)
    return 1;

  struct buffer
  {
    u_char addr[2];
    char data[128];
  } rd;

  rd.addr[0] = (_pg * 32) >> 8;
  rd.addr[1] = _pg * 32;

  Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, rd.addr, 2);
  int _ = Kernel::OS.IICDriver.IICRead(this->IIC_ADDR_E2, (u_char *)&wr, 30);

  Serial.print(wr.data);
  return _;
}

/*\ ---------------------------------------------
|*| @name: ReadFT
|*| @description:
\*/
int LogData::ReadFT(int _saddr, int _eaddr, int _pagesize, int _devlock)
{
  if ((_eaddr * _pagesize) > E2_END_ADDR)
    return 1;
  if ((_saddr * _pagesize) > E2_END_ADDR)
    return 1;
  if (_saddr > _eaddr)
    return 1;
  if ((_saddr * _pagesize) > E2_END_ADDR - _pagesize)
    return 1;
  if (((_eaddr - _saddr) * _pagesize) > 512)
    return 1;

  struct buffer
  {
    u_char addr[2];
    char data[512];
  } rd;

  rd.addr[0] = (_saddr * _pagesize) >> 8;
  rd.addr[1] = _saddr * _pagesize;

  Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, rd.addr, 2);
  int _ = Kernel::OS.IICDriver.IICRead(this->IIC_ADDR_E2, (u_char *)&wr, ((_eaddr - _saddr) * _pagesize));

  Serial.print(wr.data);
  return _;
}

/*\ ---------------------------------------------
|*| @name: ReadFT
|*| @description:
\*/
int LogData::ReadAll()
{
  int saddr[2];
  int eaddr = this->get_addr();
  char dbuffer[128];

  for (auto i = 0; i < eaddr / 128; i++)
  {
    saddr[0] = (i * 128) >> 8;
    saddr[1] = i * 128;

    Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, saddr, 2);
    iKernel::OS.IICDriver.IICRead(this->IIC_ADDR_E2, (u_char *)&dbuffer, 128);
    Serial.print(dbuffer);
  }
}