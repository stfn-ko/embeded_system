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

#define IIC_ADDR_RTC 0xDE
#define bitshift_bcd(n) ((n % 10) | ((n / 10) << 4))
#define bitshift_dec(n,m) (n & 0x0f) + (10 * ((n >> 4) & m))

class LogTask : public Kernel::Task
{
	Kernel::OSTimer tm;

	void LogToSerial(char *message);

public:
	LogTask(int _timestamp = 1000);

	virtual void TaskLoop();

	int SetDate(int dow, int day, int month, int year, int hrs, int mins, int secs, bool is24hr, bool ampm);
};

#endif
