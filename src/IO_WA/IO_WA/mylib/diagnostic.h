/*
* diagnostic.h
*
* Created: 16-11-2016 21:41:12
* Author: Brian
*/

// This lib modified from Richard's sample code
// Changed the initialise_timing.. removed USART0...
// Changed configure Time4 for 16Mhz CPU and no prescaler
// Changed TimingDiagnostic_Display_CheckPoint_DataViaUSART0() for display microsecond precision

#ifndef __DIAGNOSTIC_H__
#define __DIAGNOSTIC_H__

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include <string.h>
#include "usart_library.h"

#define CR 0x0D
#define LF 0x0A
#define SPACE 0x20
#define UC unsigned char
#define CheckPointArraySize (int)1000

void Initialise_TimingDiagnostic();;
void ConfigureTimer4_for_1msPrecisionCheckPointing();
void TimingDiagnostic_ResetTimer4();
void TimingDiagnostic_CheckPoint(UC iCheckPointArrayIndex);
void TimingDiagnostic_Display_CheckPoint_DataViaUSART0(int iLimit_ArrayIndex);

int g_iCheckpointArray[CheckPointArraySize];
UC g_iCheckPointArrayIndex; // Points to the next position to be written in the CheckPoint Array
// *** End of TimingDiagnostic declarations ***

#endif //__DIAGNOSTIC_H__
