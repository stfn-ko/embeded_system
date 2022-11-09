/*\ ---------------------------------------------
|*| @name: ENGD3051_MASTER.ino
|*| @description: This is the main module showing an encapsulated LED driver.
|*| The module containing the LED driver itself is in led.h/led.cpp
|*| The module containing the controller that passes messages to the LED to
|*| change its state is in control.h/control.cpp
|*| @INFO: FOR CONTEXT CHECK OUT INCLUDE FILES
\*/
#include "ToggleLED.h"
#include "LogTask.h"
#include "LogData.h"

//ToggleLED* Red = new ToggleLED(500, 0b00100000);
//ToggleLED* Green = new ToggleLED(300, 0b00001000);
//ToggleLED* Amber = new ToggleLED(1000, 0b00010000);

//LogTask Logger(1000);

LogData e2_0(0);

/*\ ---------------------------------------------
|*| @name: UserInit
|*| @description: This function is called once by the kernel on startup
|*| We use it to initialize our modules
\*/
void UserInit()
{
	Serial.begin(115200);
	//Red->Start();
  //Green->Start();
  //Amber->Start();
  //Logger.SetDate(5,6,9,22,20,58,0,true,false);
  //Logger.Start();

  e2_0.Write("dd/mm/yy,hh:mm:ss,tt,aaa,ddd\n");
  //add reset func to reset LRUa to 0
  //in get_LRUa func write address in in high address and low address bytes,
  // return int that represents it in dec 0-1996 (bc last 4 are 128B together and get rewritten every time)
}
