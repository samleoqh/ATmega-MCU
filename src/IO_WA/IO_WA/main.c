/*
 * IO_WA.c
 * course work demo project 
 * Created: 2016-11-08 21:17:38
 * Author : Ravi and Brian
 */ 
//standard lib
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "string.h"
//own lib
#include "usart_library.h"
#include "adc_library.h"

//3-part lib of lcd driver
#include "driver/lcdpcf8574.h"
#include "driver/pcf8574.h"

//declare functions
void InitialiseGeneral();
void Initialise_INT_Btn();
void InitialiseTimer5();
void ENABLE_INT_BTN();
void DISABLE_INT_BTN();
void Start_LCD();
void Initializse_LCD();
void Update_LCD();

//macro for bit operation
#define setbit(port, bit) (port) |= (1 << (bit))	// Set Bit ON
#define clearbit(port, bit) (port) &= ~(1 << (bit))	// Set Bit Off
#define blinkbit(port,bit) (port) ^= (1 <<(bit))	// XOR bit

//#define F_CPU 16000000UL
//define ADC channels 
#define CHANNEL0 0b01100000
#define CHANNEL1 0b01100001
#define CHANNEL2 0b01100010
#define CHANNEL3 0b01100011

#define SAMPLE_RATE_76K 76		//76KHz for ADC
#define FREQ_245HZ		0		//num for generate 245hz wavefrom 
#define FREQ_1KHZ		5		//num for generate 1Khz wavefrom 
#define FREQ_7_8KHZ		40		//num for generate 7.8Khz wavefrom 
#define FREQ_62_5KHZ	65		//num for generate 62.5Khz wavefrom 

#define BAUD			1000000 //baud rate default for usart setup

// define LED port mapping
#define ADC_LED		DDC1	//PORTC1
#define PWM_LED		DDC4	//PORTC4
#define RGB_LED_R	DDC0	//PORTC0
#define RGB_LED_G	DDC2	//PORTC2
#define RGB_LED_B	DDC3	//PORTC3

//define two basic operation modes
typedef enum  {ADC_MODE, PWM_MODE}  OPT_MODE; 
OPT_MODE cur_mode;

// default string length for mode information
#define STR_LEN 10	
// define a struct for store mode data
struct MODE_Info
{
	char mode_name[15];			// static mode title 
	char label_SRorFN[STR_LEN];	// static SR or FN string
	char label_CHorPW[STR_LEN];	// static CH or PW string
	char mode_state[STR_LEN];	// start or stop state
	char data_chpw[STR_LEN];	// real time channel or pulse width data
	char data_srfn[STR_LEN];	// real time sample rate or freq data
};

//declare two mode info struct var
struct MODE_Info adc_info;
struct MODE_Info pwm_info;
//declare a struct pointer link to on going mode
static struct MODE_Info* mode_info = &adc_info;

//define 2 actions for button 3 
typedef enum {STOP, START} BTN_ACTION;			
BTN_ACTION btn3_action = START;

int freq = 5;			//default 5kHz
int pulse_width = 1;	//default 1
int sample_rate = 76;	//default 76kHz
unsigned char new_channel = CHANNEL0;

unsigned char ADC_VAL = 0;
unsigned char channel_count = 0;
unsigned char freq_count = 0;


void Initializse_LCD()
{
	lcd_init(LCD_DISP_ON);	// initial lcd driver
	
	// initial default mode information
	strcpy(adc_info.mode_name,"ADC_MODE");
	strcpy(adc_info.label_SRorFN,"SR:");
	strcpy(adc_info.label_CHorPW,"CH:");
	strcpy(adc_info.mode_state,"stop");
	strcpy(adc_info.data_srfn,"76K  ");
	strcpy(adc_info.data_chpw,"0  ");

	strcpy(pwm_info.mode_name,"PWM_MODE");
	strcpy(pwm_info.label_SRorFN,"FN:");
	strcpy(pwm_info.label_CHorPW,"PW:");
	strcpy(adc_info.mode_state,"stop");
	strcpy(pwm_info.data_srfn,"62.5K");
	strcpy(pwm_info.data_chpw,"20 ");

	// lit on LCD 
	lcd_home();
	lcd_led(0);
}

int main(void)
{
	InitialiseGeneral();
	Initialise_INT_Btn();
	Initializse_LCD();
	InitialiseTimer5();
	Initialise_ADC();
	Initialise_Usart(BAUD); // for 16MHz, BAUD rate- 1000000
	ENABLE_INT_BTN();
	setSamplerateADC(SAMPLE_RATE_76K);
	startADC();	

	while (1)
	{
		Update_LCD(); 
	}
}

void Update_LCD()
{
	//static information
	lcd_gotoxy(0, 0);
	lcd_puts (mode_info->mode_name);
	lcd_gotoxy(0, 1);
	lcd_puts (mode_info->label_SRorFN);
	lcd_gotoxy(9, 1);
	lcd_puts (mode_info->label_CHorPW);

	//changeable information
	lcd_gotoxy(9,0);
	lcd_puts(mode_info->mode_state);
	lcd_gotoxy(3, 1);
	lcd_puts(mode_info->data_srfn);
	lcd_gotoxy(12,1);
	lcd_puts(mode_info->data_chpw);
}



void InitialiseGeneral()
{
	//Port for connecting switches
	DDRB = 0x00;			// Configure PortB direction for Output
	PORTB = 0x00;			
	//Port for connecting output LED
	DDRC=0xFF;
	PORTC=0x00;				// Set all LEDs initially off (inverted on the board, so '1' = off)
	//DDRD &=(0<<PD2);
	//DDRD &=(0<<PD3);
	//DDRE &=(0<<PE4);
	cur_mode = ADC_MODE;
	setbit(PORTC,ADC_LED);
	sei();					// Enable interrupts at global level set Global Interrupt Enable (I) bit
}

void InitialiseTimer5()		// Configure to generate an interrupt after a 2-Second interval
{
	TCCR5A = 0b00000000;	// Normal port operation (OC1A, OC1B, OC1C), Clear Timer on 'Compare Match' (CTC) waveform mode)
	TCCR5B = 0b00001101;	// CTC waveform mode, use prescaler 1024
	TCCR5C = 0b00000000;
	
	//OCR5AH = 0x3D; // Output Compare Registers (16 bit) OCR1BH and OCR1BL
	//OCR5AL = 0x09; // 1 seconde for 16MhZ CPU 16M/1024 = 0x3D09
	OCR5AH = 0x1E;
	OCR5AL = 0x84;  // 0.5 second
	TCNT5H = 0b00000000;	// Timer/Counter count/value registers (16 bit) TCNT1H and TCNT1L
	TCNT5L = 0b00000000;
	TIMSK5 = 0b00000010;	// bit 1 OCIE1A		Use 'Output Compare A Match' Interrupt, i.e. generate an interrupt
	// when the timer reaches the set value (in the OCR1A registers)
}

ISR(TIMER5_COMPA_vect) // TIMER5_CompareA_Handler (Interrupt Handler for Timer 5)
{
	
	if (btn3_action == START)
	{
		if(cur_mode == ADC_MODE)
		{
			unsigned char temp = ADCH; // read channel0 ADC value
			if (temp < 50){
				sample_rate = 9;
				strncpy(adc_info.data_srfn," 9K  ", STR_LEN); 
			}
			else if (temp < 100){
				sample_rate = 19;
				strncpy(adc_info.data_srfn,"19K  ", STR_LEN);
			}
			else if (temp < 150){
				sample_rate = 38;
				strncpy(adc_info.data_srfn,"38K  ", STR_LEN);
			}
			else if (temp < 200){
				sample_rate = 76;
				strncpy(adc_info.data_srfn,"76K  ", STR_LEN);
			}
			else{
				sample_rate = 76;
				strncpy(adc_info.data_srfn,"76K  ", STR_LEN);//152; may cause some problem
			}
		}
		else
		{
			if (cur_mode == PWM_MODE)
			{
				pulse_width = ADC_VAL;
				//itoa(pulse_width, pwm_info.data_chpw, STR_LEN); 
			}
		}
	}else
	{
		blinkbit(PORTC,PWM_LED);
	}
	// update LCD display
}

ISR(ADC_vect)	// ADC Interrupt Handler
{
    USART_TX_SingleByte(ADCH);
}

// Initialize 3 INT (2,3,4) for Button events
void Initialise_INT_Btn()
{
	// Interrupt Sense (INT2 & INT3) falling-edge triggered for Button 1 & 2
	EICRA = 0b10100000;		
	// Interrupt Sense (INT4) falling-edge triggered for Button 3
	EICRB = 0b00000010; 
	// Initially disabled, 
	EIMSK = 0b00000000;		
	// Clear all HW interrupt flags 
	EIFR = 0b11111111;		
}

void ENABLE_INT_BTN()
{
	EIMSK = 0b00011100;		// Enable H/W Int 2,3,4 for Button 1, 2, 3
}

void DISABLE_INT_BTN()
{
	EIMSK = 0b00000000;		// Disable H/W Int 
}


// Button 1 INT event
ISR(INT2_vect) // Interrupt Handler for H/W INT 2 - 
{
	DISABLE_INT_BTN();
	if (cur_mode == ADC_MODE) 
	{
		cur_mode = PWM_MODE;
		setbit(PORTC,PWM_LED);	// lit on PWM LED to indicate PWM_MODE on 4
		clearbit(PORTC,ADC_LED);	// lit off ADC LED 1		
		ADMUX = CHANNEL0;
		strncpy(adc_info.data_chpw,"0  ", STR_LEN); 
		channel_count = 0;
		mode_info = &pwm_info;
	}
	else
	{
		cur_mode = ADC_MODE;
		setbit(PORTC,ADC_LED);
		clearbit(PORTC,PWM_LED);
		mode_info = &adc_info;
	}
	//_delay_ms(100);	
	ENABLE_INT_BTN();
} 

// Button 2 INT event
ISR(INT3_vect) 
{
	DISABLE_INT_BTN();		// Disable INT0 to prevent key bounce causing multiple interrupt events
	if (cur_mode == PWM_MODE)
	{
		switch(freq_count)
		{
			case 0:
				freq = FREQ_245HZ;
				strncpy(pwm_info.data_srfn,"245Hz", STR_LEN); 
			break;
			case 1:
				freq = FREQ_1KHZ;
				strncpy(pwm_info.data_srfn,"1KHz ", STR_LEN);
			break;
			case 2:
				freq = FREQ_7_8KHZ;
				strncpy(pwm_info.data_srfn,"7.8Kh", STR_LEN);
			break;
			case 3:
				freq = FREQ_62_5KHZ;
				strncpy(pwm_info.data_srfn,"62.5K", STR_LEN);
			break;
		}

		freq_count++;

		if (freq_count > 3)
			freq_count = 0;
	}
	else
	{
		switch(channel_count)
		{
			case 0:
				setbit(PORTC,RGB_LED_R);	// Lit ON RGB - Red
				setbit(PORTC,RGB_LED_G);	// Lit ON RGB - Green
				setbit(PORTC,RGB_LED_B);	// Lit ON RGB - Blue
				new_channel = CHANNEL0;
				strncpy(adc_info.data_chpw,"0  ", STR_LEN);	
			break;
			case 1:
				clearbit(PORTC,RGB_LED_R);	// Lit Off Red
				clearbit(PORTC,RGB_LED_B);  // Lit ON RGB - Blue
				new_channel = CHANNEL1;
				strncpy(adc_info.data_chpw,"1  ", STR_LEN);
			break;
			case 2:
				clearbit(PORTC,RGB_LED_G);	// Lit Off Green
				setbit(PORTC,RGB_LED_B);	// Lit ON RGB - Blue
				new_channel = CHANNEL2;
				strncpy(adc_info.data_chpw,"2  ", STR_LEN);
			break;
			case 3:
				clearbit(PORTC,RGB_LED_B); // Lit Off Blue
				setbit(PORTC,RGB_LED_R);
				new_channel = CHANNEL3;
				strncpy(adc_info.data_chpw,"3  ", STR_LEN);
			break;
		}

		channel_count++;

		if (channel_count > 3)
		channel_count = 0;
	}

	ENABLE_INT_BTN();// Re-enable the interrupt
}

// Button 3 INT event
ISR(INT4_vect) // Interrupt Handler for H/W INT 4 - Button 3
{
	DISABLE_INT_BTN();	// Disable INT to prevent key bounce 
	switch(btn3_action)
	{
		case STOP:
			ADMUX = CHANNEL0;			
			strncpy(adc_info.data_chpw,"0  ", STR_LEN); 
			strcpy(adc_info.mode_state,"stop  ");
			if (cur_mode == ADC_MODE)
			{
				new_channel = CHANNEL0;
				channel_count = 1;
			}
			else //in PWM mode
			{
				strcpy(pwm_info.mode_state,"stop  ");
			}
			btn3_action = START;
			
		break;
		case START:
			if (cur_mode == ADC_MODE)
			{
				ADMUX = new_channel;
				setSamplerateADC(sample_rate);
				strcpy(adc_info.mode_state,"start");
			}
			else  //in PWM mode
			{
				strcpy(pwm_info.mode_state,"start");
			}
			btn3_action = STOP;
		break;
	}

	ENABLE_INT_BTN();		// Re-enable the interrupt
}


