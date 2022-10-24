///////////////////////////////////////////////////////////////////////////////
// SYSINCS.H
//
// System includes
//
// Dr J A Gow 2020
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SYSINCS_H_
#define _SYSINCS_H_

#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>
#include "interrupts.h"
#include "EventReceiver.h"

// Boolean type

//typedef	uint8_t	boolean;
//enum { false, true };

#define IMIN(a,b) (((a)<(b))?(a):(b))
#define IMAX(a,b) (((a)>(b))?(a):(b))

extern "C" void loop();

#endif
