/*\ ---------------------------------------------
|*| @name: TOGGLETASK.H
|*| @author: Dr J A Gow
|*| @editor: BCs Stephan Kolontay
|*| @date: 10/10/2022
|*| @description: Derived from Kernel::Task, this is a specialist task to toggle an entity
|*|               with a predefined rate. The timeout is specified in the constructor, the hardware interaction
|*|               by a defined class
\*/

#ifndef TOGGLETASK_H_
#define TOGGLETASK_H_

#include "kernel.h"

class ToggleTask : public Kernel::Task {

		Kernel::OSTimer tm;
		boolean toggleflag;

	public:

    /*\ ---------------------------------------------
    |*| @name: ToggleTask
    |*| @description: Constructor. Sets up the task with a LED number and timeout
		\*/ 
		ToggleTask(unsigned long timeout);

    /*\ ---------------------------------------------
    |*| @name: TaskLoop
    |*| @description: Main task loop - flash the LED
    |*| @scope: PUBLIC
    |*| @context: TASK
    |*| @param: none
    |*| @return: none
    \*/ 
		virtual void TaskLoop();

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
		virtual void Toggle(boolean state);

};

#endif 
