  /*\ ---------------------------------------------
  |*| @name: LogData.H
  |*| @Note: 0-write/1-read
  |*| @INFO: FOR CONTEXT CHECK OUT INCLUDE FILES
  \*/
#include "LogData.h"


  /*\ ---------------------------------------------
  |*| @name: LogData
  |*| @description: parameterized constructor that takes address of E2 device from the board;
  |*| @PARAM: _port_addr describes the type of wiring and ranges from 0-7, defaluted to 0
  \*/
LogData::LogData(uint8_t _port_addr) : IIC_ADDR_E2(0xA0 | _port_addr << 1) {}


  /*\ ---------------------------------------------
  |*| @name: get_addr
  |*| @description: gets last recently used address
  \*/
int LogData::get_addr() {
  struct dbuff{
    u_char addr[2];
    char data[4];
  } rd;
  
  rd.addr[0] = this->L_ADDR >> 8;
  rd.addr[1] = this->L_ADDR;

  Kernel::OS.IICDriver.IICWrite(IIC_ADDR_E2, rd.addr, 2);
  Kernel::OS.IICDriver.IICRead(IIC_ADDR_E2, (u_char *)&rd.data, 4);
  int _ = (rd.data[0] - 48) * 1000;
  _ += (rd.data[1] - 48) * 100;
  _ += (rd.data[2] - 48) * 10;
  _ += rd.data[3] - 48;

  Serial.println(rd.data);
  return _;
}


  /*\ ---------------------------------------------
  |*| @name: upd_addr 
  |*| @description: updates last recently used address
  |*| @STATUS: tested : works
  \*/
void LogData::upd_addr(int _caddr) {
  ++_caddr;
  
  if(_caddr >= 2044)
    _caddr = 0;
  
  int _[4];
  
  struct upload{
    u_char addr[2];
    char dbuff[4];
  } upd;

  upd.addr[0] = this->L_ADDR >> 8;
  upd.addr[1] = this->L_ADDR;

  
  for(auto i = 3; i >= 0 ; --i)
  {
    _[i] = _caddr % 10;
    _caddr = _caddr / 10;
  }

  for(auto i = 0; i < 4; ++i)
    upd.dbuff[i] = _[i] + 48;
  
  Serial.print("Update Address - final upd value: ");Serial.println(upd.dbuff);
  Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, (u_char*)&upd, 6);
}



  /*\ ---------------------------------------------
  |*| @name: Reset
  |*| @description: Resets last recently used address to 0
  |*| @STATUS: tested : works
  \*/
void LogData::Reset(int _dl) {
  Write(2044, "0000", _dl);
} 


  /*\ ---------------------------------------------
  |*| @name: Write
  |*| @description: writes consequently data to EEPROM
  |*| @NOTE: 
  \*/
int LogData::WriteAuto(c_char* _data) {
  int _a = get_addr();
  
  Serial.print("WriteAuto - addr1: "); Serial.println(_a);
  if(_a >= 2044)
    _a = 0;
  
  Serial.print("WriteAuto - addr2: "); Serial.println(_a);
  int _ = Write(_a, _data);
  Serial.print("WriteAuto - writing result: ");Serial.println(_);
  
  if(!_)
    this->upd_addr(_a);

  return _;
}


  /*\ ---------------------------------------------
  |*| @name: Write
  |*| @description: writes consequently data to EEPROM
  |*| @NOTE: 
  \*/
int LogData::Write(int _page, c_char* _data, int _dl) {
    if(ok(_pg, _dl))
      return 1;
    
  struct dbuff{
    u_char addr[2];
    char data[32];
  } wr;
  
  wr.addr[0] = (_page * 32) >> 8; 
  wr.addr[1] = _page * 32; 
  strcpy(wr.data, _data);

  return Kernel::OS.IICDriver.IICWrite(this->IIC_ADDR_E2, (u_char*)&wr,  34);
}

bool LogData::ok(int _a, int _d){
  if((_a >= 2044) && (_d != DEVLOCK)){
    Serial.println("ERROR: Invalid address!\nYou can only enter values between 0 and 2043, unless you have the devlock.");
    return 1;
  }
  
  if ( _d == DEVLOCK)
    Serial.println("VALID: DEVLOCK");
  
  return 0;
}

  /*\ ---------------------------------------------
  |*| @name: ReadPage
  |*| @description: Reads all pages
  \*/
int LogData::ReadAll() {
  int _ = 0;
  for (auto i = 0; i < 2044; ++i)
    _ = ReadPage(i);
  return _;
}


  /*\ ---------------------------------------------
  |*| @name: ReadPage
  |*| @description: Reads 32B page from addr
  |*| @TODO: add _format function to println for values that dont have '\n' at the end. 
  |*| : Maybe format like CSV?
  \*/
int LogData::ReadPage(int _pg, int _dl) {

  if(ok(_pg, _dl))
    return 1;
  
  struct dbuff{
    u_char addr[2];
    char data[32];
  } rd;
  
  rd.addr[0] = (_pg * 32) >> 8;
  rd.addr[1] = _pg * 32;

  Kernel::OS.IICDriver.IICWrite(IIC_ADDR_E2, rd.addr, 2);
  int _ = Kernel::OS.IICDriver.IICRead(IIC_ADDR_E2, (u_char *)&rd.data, 32);
  Serial.print(rd.data);
  return _;
}


  /*\ ---------------------------------------------
  |*| @name: ReadFT
  |*| @description: Reads from - to
  \*/
int LogData::ReadFT(int _saddr, int _eaddr, int _dl) {
  if ((_eaddr * 32) > E2_END_ADDR)
   return 1;
  if ((_saddr * 32) > E2_END_ADDR)
   return 1;
  if (_saddr > _eaddr)
   return 1;
  if ((_saddr * 32) > E2_END_ADDR - 32)
   return 1;
  
  int _ = 1;
  for (auto i = _saddr; i <= _eaddr; ++i)
    _ = ReadPage(i);
    
  return _;
}
