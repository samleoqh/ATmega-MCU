/*
 * IO_WA.c
 * course work demo project 
 * Created: 2016-11-08 21:17:38
 * Author : Ravi and Brian
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
void ButtonEvents();
void Start_LCD();
void InitialiseTimer5()	;

#define setbit(port, bit) (port) |= (1 << (bit))
#define clearbit(port, bit) (port) &= ~(1 << (bit))

#define CHANNEL0 0b01100000
#define CHANNEL1 0b01100001
#define CHANNEL2 0b01100010
#define CHANNEL3 0b01100011

int btn_count = 0;
bool pressed = false;
bool adc_stopped = true;
bool change_ch = false;
char strbuf[10];
unsigned char new_ch;

int sample_rate = 9;
unsigned char sample_flag = 0;

int main(void)
{
	/* Replace with your application code */

	InitialiseGeneral();
	Start_LCD();
	//USART_SETUP_9600_BAUD_ASSUME_1MHz_CLOCK();
	USART_SETUP_BAUD_ASSUME_16MHz_CLOCK(1000000);
	Initialise_ADC();
	setSamplerateADC(152);
	InitialiseTimer5();
	startADC();

	while (1)
	{
		ButtonEvents();
	}
	return 1;
}


void Start_LCD()
{
	lcd_init(LCD_DISP_ON);
	lcd_home();
	lcd_puts("**Welcome OSC**");
	lcd_led(0);
	lcd_gotoxy(0, 1);
	lcd_puts ("SR:");
	lcd_gotoxy(5, 1);
	lcd_puts ("k");
	lcd_gotoxy(9, 1);
	lcd_puts ("CHAN:");
	//lcd_gotoxy(14, 1);
	//lcd_puts ("hz");
}

void ButtonEvents()
{
	unsigned char temp;	
	if (PINB & 0x01)
	{
		if(pressed)
		{
			btn_count++;
			pressed = false;
		}
	}		
	else 
	{
		if(!pressed)	
			pressed = true;			
	}
	
	temp = btn_count % 2;

	PORTC = (PORTC & ~0x01) | (temp & 0x01); // lit on or off 

	if (temp & 0x01)
	{
		if (adc_stopped)
		{
			startADC();
			change_ch = true;
			new_ch = CHANNEL1;
			//ADMUX = CHANNEL1;	
			adc_stopped = false;
		}
	}
	else
	{	
		if (!adc_stopped)
		{
			//stopADC();
			change_ch = true;
			new_ch = CHANNEL0;	
			adc_stopped = true;
		}
	}

	if(adc_stopped)
	{
		unsigned char temp = sample_flag;
		if (temp < 50)
		   sample_rate = 9;
		else if (temp < 100)
			sample_rate = 19;
		else if (temp < 150)
			sample_rate = 38;
		else if (temp < 200)
			sample_rate = 76;
		else
			sample_rate = 152;
	}
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

void InitialiseTimer5()		// Configure to generate an interrupt after a 2-Second interval
{
	TCCR5A = 0b00000000;	// Normal port operation (OC1A, OC1B, OC1C), Clear Timer on 'Compare Match' (CTC) waveform mode)
	TCCR5B = 0b00001101;	// CTC waveform mode, use prescaler 1024
	TCCR5C = 0b00000000;
	
	OCR5AH = 0x7A; // Output Compare Registers (16 bit) OCR1BH and OCR1BL
	OCR5AL = 0x01; //

	TCNT5H = 0b00000000;	// Timer/Counter count/value registers (16 bit) TCNT1H and TCNT1L
	TCNT5L = 0b00000000;
	TIMSK5 = 0b00000010;	// bit 1 OCIE1A		Use 'Output Compare A Match' Interrupt, i.e. generate an interrupt
	// when the timer reaches the set value (in the OCR1A registers)
}
	
ISR(TIMER5_COMPA_vect) // TIMER1_CompareA_Handler (Interrupt Handler for Timer 1)
{
	if (ADMUX == CHANNEL0)
	{
		lcd_gotoxy(3, 1);
		lcd_puts("  ");
		lcd_gotoxy(3, 1);
		itoa(sample_rate, strbuf, 10);
		lcd_puts(strbuf);
	}
	lcd_gotoxy(14,1);
	//itoa(freq, strbuf, 10);
	lcd_puts("1");
}


ISR(ADC_vect)	// ADC Interrupt Handler
{
	unsigned char ADMUX_temp = ADMUX;

	if (change_ch)
	{
		ADMUX = new_ch;
		change_ch = false;
	}

	if (ADMUX_temp == CHANNEL0)
		sample_flag = ADCH;
	else if (ADMUX_temp == CHANNEL1)
		USART_TX_SingleByte(ADCH);
}