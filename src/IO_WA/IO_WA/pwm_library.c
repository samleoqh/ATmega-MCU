/*
 * pwm_library.c
 *
 * Created: 17-11-2016 12:23:37
 *  Author: ravim
 */ 

#include <avr/io.h>

#include "pwm_library.h"

 void changePWMDutyCycle(int pwm,int pulseWidth)
 {
	 switch (pwm)
	 {
		 case 0:
		 OCR0A=pulseWidth;
		 break;
		 case 1:
		 OCR0B=pulseWidth;
		 break;
		 case 3:
		 OCR2A=pulseWidth;
		 break;
		 case 4:
		 OCR2B=pulseWidth;
		 break;
	 }
 }

 void enableInvertingPWM(int pwm)
 {

	 switch (pwm)
	 {
		 case 0:
		 TCCR0A |=(1<<COM0A1);	//INVERTING MODE
		 TCCR0A |=(1<<COM0A0);
		 break;
		 case 1:
		 TCCR0A |=(1<<COM0B1);	//INVERTING MODE
		 TCCR0A |=(1<<COM0B0);
		 break;
		 case 3:
		 TCCR2A |=(1<<COM2A1);	//INVERTING MODE
		 TCCR2A |=(1<<COM2A0);
		 break;
		 case 4:
		 TCCR2A |=(1<<COM2B1);	//INVERTING MODE
		 TCCR2A |=(1<<COM2B0);
		 break;
	 }
 }
 void enablePWM(int number , int pulseWidth, int frequency)
 {
	 // DEFAULT VALUE FOR TIMER 0 AND 2
	 TCNT0=0X00;
	 TCNT2=0X00;

	 switch (number)
	 {
		 case 0:					//SWITCH ON THE PWM 0 BY ENABLING THE PB7 AS OUTPUT
		 DDRB|=(1<<PB7);
		 OCR0A=pulseWidth;		//TIMER 0 COMPARE VALUE A IS SET FOR PULSE WIDTH

		 TCCR0A |=(1<<COM0A1);	//NON INVERTING MODE
		 TCCR0A &=~(1<<COM0A0);

		 TCCR0A |=(1<<WGM01);	//FAST PWM
		 TCCR0A |=(1<<WGM00);
		 TCCR0B &=~(1<<WGM02);
		 
		 TCCR0B &=~(1<<FOC0A);	//DISABLE FORCED OUTPUT COMPARE
		 TCCR0B &=~(1<<FOC0B);	//DISABLE FORCED OUTPUT COMPARE

		 if (frequency > 50)
		 {
			 TCCR0B &=~(1<<CS02);		//NO PRESCALAR
			 TCCR0B &=~(1<<CS01);
			 TCCR0B |=(1<<CS00);
		 }
		 else if (frequency> 7)
		 {
			 TCCR0B &=~(1<<CS02);		//PRESCALAR 8
			 TCCR0B |=(1<<CS01);
			 TCCR0B &=~(1<<CS00);
		 }
		 else if (frequency< 7 && frequency > 1)
		 {
			 TCCR0B &=~(1<<CS02);		//PRESCALAR 64
			 TCCR0B |=(1<<CS01);
			 TCCR0B |=(1<<CS00);
		 }
		 else if(frequency<1)
		 {
		 	 //Default prescalar FOR TIMER 0
		 	 TCCR0B |=(1<<CS02);		//PRESCALAR 256
		 	 TCCR0B &=~(1<<CS01);
		 	 TCCR0B &=~(1<<CS00);
		 }

		 break;
		 case 1:
		 DDRG|=(1<<PG5);			//SWITCH ON THE PWM 1 BY ENABLING THE PG5 AS OUTPUT
		 OCR0B=pulseWidth;		//TIMER 0 COMPARE VALUE B IS SET FOR PULSE WIDTH

		 TCCR0A |=(1<<COM0B1);	//NON INVERTING MODE
		 TCCR0A &=~(1<<COM0B0);

		 TCCR0A |=(1<<WGM01);	//FAST PWM
		 TCCR0A |=(1<<WGM00);
		 TCCR0B &=~(1<<WGM02);

		 TCCR0B &=~(1<<FOC0A);	//DISABLE FORCED OUTPUT COMPARE
		 TCCR0B &=~(1<<FOC0B);	//DISABLE FORCED OUTPUT COMPARE

		 if (frequency > 50)
		 {
			 TCCR0B &=~(1<<CS02);		//NO PRESCALAR
			 TCCR0B &=~(1<<CS01);
			 TCCR0B |=(1<<CS00);
		 }
		 else if (frequency> 7)
		 {
			 TCCR0B &=~(1<<CS02);		//PRESCALAR 8
			 TCCR0B |=(1<<CS01);
			 TCCR0B &=~(1<<CS00);
		 }
		 else if (frequency< 7 && frequency > 1)
		 {
			 TCCR0B &=~(1<<CS02);		//PRESCALAR 64
			 TCCR0B |=(1<<CS01);
			 TCCR0B |=(1<<CS00);
		 }
			else if(frequency<1)
			{
				//Default prescalar FOR TIMER 0
				TCCR0B |=(1<<CS02);		//PRESCALAR 256
				TCCR0B &=~(1<<CS01);
				TCCR0B &=~(1<<CS00);
			}

		 break;
		 case 2:
		 DDRB|=(1<<PB4);			//SWITCH ON THE PWM 2 BY ENABLING THE PB4 AS OUTPUT
		 OCR2A=pulseWidth;		//TIMER 2 COMPARE VALUE A IS SET FOR PULSE WIDTH

		 TCCR2A |=(1<<COM2A1);	//NON INVERTING MODE
		 TCCR2A &=~(1<<COM2A0);

		 TCCR2A |=(1<<WGM21);	//FAST PWM
		 TCCR2A |=(1<<WGM20);
		 TCCR2B &=~(1<<WGM22);

		 TCCR2B &=~(1<<FOC2A);	//DISABLE FORCED OUTPUT COMPARE
		 TCCR2B &=~(1<<FOC2B);	//DISABLE FORCED OUTPUT COMPARE

		 if (frequency > 50)
		 {
			 TCCR2B &=~(1<<CS22);		//NO PRESCALAR
			 TCCR2B &=~(1<<CS21);
			 TCCR2B |=(1<<CS20);
		 }
		 else if (frequency> 7)
		 {
			 TCCR2B &=~(1<<CS22);		//PRESCALAR 8
			 TCCR2B |=(1<<CS21);
			 TCCR2B &=~(1<<CS20);
		 }
		 else if (frequency< 7 && frequency > 1)
		 {
			 TCCR2B &=~(1<<CS22);		//PRESCALAR 64
			 TCCR2B |=(1<<CS21);
			 TCCR2B |=(1<<CS20);
		 }
		 else if(frequency <1)
		 {
		 	 //Default prescalar FOR TIMER 2
		 	 TCCR2B |=(1<<CS22);		//PRESCALAR 256
		 	 TCCR2B &=~(1<<CS21);
		 	 TCCR2B &=~(1<<CS20);
		 }
		 break;
		 case 3:
		 DDRH|=(1<<PH6);			//SWITCH ON THE PWM 3 BY ENABLING THE PH6 AS OUTPUT
		 OCR2B=pulseWidth;		//TIMER 2 COMPARE VALUE B IS SET FOR PULSE WIDTH

		 TCCR2A |=(1<<COM2B1);	//NON INVERTING MODE
		 TCCR2A &=~(1<<COM2B0);

		 TCCR2A |=(1<<WGM21);	//FAST PWM
		 TCCR2A |=(1<<WGM20);
		 TCCR2B &=~(1<<WGM22);

		 TCCR2B &=~(1<<FOC2A);	//DISABLE FORCED OUTPUT COMPARE
		 TCCR2B &=~(1<<FOC2B);	//DISABLE FORCED OUTPUT COMPARE

		 if (frequency > 50)
		 {
			 TCCR2B &=~(1<<CS22);		//NO PRESCALAR
			 TCCR2B &=~(1<<CS21);
			 TCCR2B |=(1<<CS20);
		 }
		 else if (frequency> 7)
		 {
			 TCCR2B &=~(1<<CS22);		//PRESCALAR 8
			 TCCR2B |=(1<<CS21);
			 TCCR2B &=~(1<<CS20);
		 }
		 else if (frequency< 7 && frequency > 1)
		 {
			 TCCR2B &=~(1<<CS22);		//PRESCALAR 64
			 TCCR2B |=(1<<CS21);
			 TCCR2B |=(1<<CS20);
		 }
		else if(frequency <1)
		{
			//Default prescalar FOR TIMER 2
			TCCR2B |=(1<<CS22);		//PRESCALAR 256
			TCCR2B &=~(1<<CS21);
			TCCR2B &=~(1<<CS20);
		}

		 break;
	 }



 }
