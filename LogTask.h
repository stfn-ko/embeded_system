/*\ ---------------------------------------------
|*| @name: LOGTASK.H
|*| @author: Dr J A Gow / Dr M A Oliver 2022
|*| @editor: BCs Stephan Kolontay
|*| @date: 10/10/2022
|*| @description: Derived from Task - this uses a timer to write the RTC value to the
|*| display at periodic intervals
\*/ 

#ifndef LOGTASK_H_
#define LOGTASK_H_

#include "kernel.h"

class LogTask : public Kernel::Task {

		Kernel::OSTimer tm;

		void LogToSerial(char * message); 
                
	public:
 
		LogTask(int _timestamp = 1000);
    
		virtual void TaskLoop();

		int SetDate(int dow, int day, int month, int year, int hrs, int mins, int secs, boolean is24hr, boolean ampm);
};

#endif
