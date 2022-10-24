/*\ ---------------------------------------------
|*| @name: TOGGLETASK.CPP
|*| @INFO: FOR CONTEXT CHECK OUT INCLUDE FILES
\*/
#include "ToggleTask.h"


/*\ ---------------------------------------------
|*| @purpose: LED Task
|*| @description: Constructor. Sets up the task with a LED number and timeout
\*/
ToggleTask::ToggleTask(unsigned long timeout) : tm(timeout), toggleflag(false){}

/*\ ---------------------------------------------
|*| @name: TaskLoop
|*| @description: Main task loop - flash the LED
|*| @scope: PUBLIC
|*| @context: TASK
|*| @param: none
|*| @return: none
\*/
void ToggleTask::TaskLoop()
{
	if(tm.isExpired()) {
		Toggle(toggleflag);
		toggleflag=(toggleflag)?false:true;
		tm.Restart();
	}
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
void ToggleTask::Toggle(boolean state)
{
	Serial.println("base class called: function not implemented\n");
}
