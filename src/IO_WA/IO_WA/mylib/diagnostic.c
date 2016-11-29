/*
* diagnostic.c
*
* Created: 16-11-2016 21:40:10
* Author: Brian
*/
// This lib modified from Richard's sample code
// Changed the initialise_timing.. removed USART0...
// Changed configure Time4 for 16Mhz CPU and no prescaler
// Changed TimingDiagnostic_Display_CheckPoint_DataViaUSART0() for display microsecond precision

#include "diagnostic.h"
#include "usart_library.h"
#include <stdio.h>
#include <stdlib.h>
// *** The Timing Diagnostic support methods are defined from here onwards ***
void Initialise_TimingDiagnostic()
{
	ConfigureTimer4_for_1msPrecisionCheckPointing();
	//USART0_SETUP_9600_BAUD_ASSUME_1MHz_CLOCK();
}

void ConfigureTimer4_for_1msPrecisionCheckPointing()	// Configure to generate an interrupt every 1 MilliSecond (actually every 1.024 mS)
// Thus can time events up to 2^16 milliseconds = approximately 1000 seconds, with 1.024ms precision
{
	TCCR4A = 0b00000000;	// Normal port operation (OC4A, OC4B, OC4C), Normal waveform mode)
	//TCCR4B = 0b00000101;	// Normal waveform mode, use prescaler /1024
	TCCR4B = 0b00000001;	// Normal waveform mode, no prescaler 
	//TCCR4B = 0b00000011;	// Normal waveform mode, use prescaler 64 for 16Mhz clock
	TCCR4C = 0b00000000;

	OCR4AH = 0x00; // Output Compare Registers (16 bit) OCR4AH and OCR4AL
	OCR4AL = 0x00;
	OCR4BH = 0x00; // Output Compare Registers (16 bit) OCR4BH and OCR4BL
	OCR4BL = 0x00;

	TCNT4H = 0x00;	// Timer/Counter count/value registers (16 bit) TCNT4H and TCNT4L
	TCNT4L = 0x00;
	TIMSK4 = 0b00000000;	// Not using interrupts
}

void TimingDiagnostic_ResetTimer4()	// Reset count to 0
{
	TCNT4H = 0x00;	// Timer/Counter count/value registers (16 bit) TCNT4H and TCNT4L
	TCNT4L = 0x00;
}

void TimingDiagnostic_CheckPoint(UC iCheckPointArrayIndex)	// Record a CheckPoint value
{
	//int iCheckPoint_Value = (TCNT4H * 256) + TCNT4L;
	// 16Mhz, no prescale
	// minus the function call time around 18 clock cycles. 
	int iCheckPoint_Value = ((TCNT4H * 256) + TCNT4L) - 18; 
	g_iCheckpointArray[iCheckPointArrayIndex] = iCheckPoint_Value;
}

void TimingDiagnostic_Display_CheckPoint_DataViaUSART0(int iLimit_ArrayIndex)	// Display CheckPoint values
// Call this method at the end of execution to avoid disturbing timing whilst recording CheckPoints
{
	char cArrayIndex[10];
	char cArrayValue[10];
	char cDisplayString[100];
	int lMicroseconds = 0;
	USART0_DisplayBanner_Head();
	
	for(int iCount = 0; iCount < iLimit_ArrayIndex; iCount++)
	{
		itoa(iCount, cArrayIndex, 10);
		itoa(g_iCheckpointArray[iCount], cArrayValue, 10);
		//lMilliseconds = ((long)g_iCheckpointArray[iCount] * 1024) / 1000;
		lMicroseconds = (int)g_iCheckpointArray[iCount]/16; // 16MHZ no prescale, microsecond
		sprintf(cDisplayString, "          %4s        %7s    %8d", cArrayIndex, cArrayValue, lMicroseconds);
		USART0_TX_String(cDisplayString);
	}
	USART0_DisplayBanner_Tail();
}
