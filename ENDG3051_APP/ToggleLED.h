/*\ ---------------------------------------------
|*| @name: TOGGLELED.H
|*| @author: Dr J A Gow
|*| @editor: BCs Stephan Kolontay
|*| @date: 10/10/2022
|*| @description: Derived from ToggleTask - this uses ToggleTask to flash an LED.
\*/

#ifndef _TOGGLELED_H_
#define _TOGGLELED_H_

#include "ToggleTask.h"

class ToggleLED : public ToggleTask {
  
  const uint8_t ddrb; 
  
	public:

    /*\ ---------------------------------------------
    |*| @name: ToggleLED
    |*| @description: Constructor. Sets up the LED hardware, and initializes the
    |*| base clase.
		\*/
		ToggleLED(unsigned long timeout, uint8_t _ddrb);

	protected:

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
		virtual void Toggle(bool state);

};

#endif 
