/*\ ---------------------------------------------
|*| @name: TOGGLELED.CPP
|*| @INFO: FOR CONTEXT CHECK OUT INCLUDE FILES
\*/
#include "ToggleLED.h"

/*\ ---------------------------------------------
|*| @name: ToggleLED
|*| @description: Constructor. Sets up the LED hardware, and initializes the
|*| base clase.
\*/
ToggleLED::ToggleLED(unsigned long _to, uint8_t _port) : ddrb(_port), ToggleTask(_to)
{
  DDRB |= ddrb;
}

/*\ ---------------------------------------------
|*| @name: Toggle
|*| @description: This function is devolved to derived classes to toggle the LED. Of
|*| course, it could be derived to do something else! It is a virtual
|*| function, but not a pure virtual (so it does nothing if called on
|*| the base class).
|*| @scope: PROTECTED
|*| @context: TASK
|*| @param: BOOLEAN state (true=on, false=off)
|*| @return: none
\*/
void ToggleLED::Toggle(bool state)
{ 
  PORTB &= ~ddrb;
  
  if(state)
		PORTB |= ddrb;
}
