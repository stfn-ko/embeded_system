////////////////////////////////////////////////////////////////////////////////
/// KERNEL.H
///
/// Kernel class
///
/// Dr J A Gow 2022
///
////////////////////////////////////////////////////////////////////////////////

#ifndef KERNEL_H_
#define KERNEL_H_

extern void UserInit(void);

#include "taskring.h"
#include "ostimer.h"
#include "iic.h"
#include "mq.h"

namespace Kernel
{
	struct KernelClass
	{
		TaskRing &TaskManager = TaskRing::Get();
		MQClass &MessageQueue = MQClass::Get();
		IIC &IICDriver = IIC::Get();
	};

	extern KernelClass OS;
}
#endif
