/*
 * IO_WA.c
 *
 * Created: 2016-11-08 21:17:38
 * Author : qhsam
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "driver/lcdpcf8574.h"
#include "driver/pcf8574.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "usart_library.h"
#include "adc_library.h"

void InitialiseGeneral();
void ButtonEvent();
void InitialiseTimer1()	;


int main(void)
{
	/* Replace with your application code */

	InitialiseGeneral();
	lcd_init(LCD_DISP_ON);
	lcd_home();
	lcd_puts("**Welcome OSC**");
	lcd_led(0);

	USART_SETUP_9600_BAUD_ASSUME_1MHz_CLOCK();
	Initialise_ADC();
	InitialiseTimer1();

	while (1)
	{
		ButtonEvent();
	}

	return 1;
}

int count = 0;
bool pressed = false;

void ButtonEvent()
{
	//button = PINB &0x01 ;
	//PORTC = button;
	//button = PINB & 0x01;

	unsigned char temp;	

	//if(button & 0x01)
	if (PINB & 0x01)
	{
		if(pressed)
		{
			count ++;
			pressed = false;
			temp = count%2;
			PORTC = temp;
		}			
		//PORTC|= 1<<0;
	}		
	else 
	{
		if(!pressed)
			pressed = true;		
		//PORTC &= ~(1<<0);
	}
	//_delay_ms(100);
}

void InitialiseGeneral()
{
	//Port for connecting switches
	DDRB = 0x00;			// Configure PortB direction for Output
	PORTB = 0x00;			// Set all LEDs initially off (inverted on the board, so '1' = off)
	//Port for connecting output LED
	DDRC=0xFF;
	PORTC=0x00;

	sei();					// Enable interrupts at global level set Global Interrupt Enable (I) bit
}

void InitialiseTimer1()		// Configure to generate an interrupt after a 2-Second interval
{
	TCCR1A = 0b00000000;	// Normal port operation (OC1A, OC1B, OC1C), Clear Timer on 'Compare Match' (CTC) waveform mode)
	TCCR1B = 0b00001101;	// CTC waveform mode, use prescaler 1024
	TCCR1C = 0b00000000;
	
	// For 1 MHz clock (with 1024 prescaler) to achieve a 2 second interval:
	// Need to count 2 million clock cycles (but already divided by 1024)
	// So actually need to count to (2000000 / 1024 =) 1953 decimal, = 7A1 Hex
	OCR1AH = 0x00; // Output Compare Registers (16 bit) OCR1BH and OCR1BL
	OCR1AL = 0x7A;

	TCNT1H = 0b00000000;	// Timer/Counter count/value registers (16 bit) TCNT1H and TCNT1L
	TCNT1L = 0b00000000;
	TIMSK1 = 0b00000010;	// bit 1 OCIE1A		Use 'Output Compare A Match' Interrupt, i.e. generate an interrupt
	// when the timer reaches the set value (in the OCR1A registers)
}

ISR(TIMER1_COMPA_vect) // TIMER1_CompareA_Handler (Interrupt Handler for Timer 1)
{
	lcd_led(0); 
	char buf[10] = "    ";
	itoa(count, buf, 10);
	lcd_gotoxy(0, 1);
	lcd_puts(buf);
	//Toggle_LED3();		// Bit 3 is toggled every time the interrupt occurs
}
