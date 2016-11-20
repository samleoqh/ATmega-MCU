/*
 * IO_WA.c
 * course work demo project 
 * Created: 2016-11-08 21:17:38
 * Author : Ravi and Brian
 */ 

//standard lib
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "string.h"
//own lib
#include "driver/usart_library.h"
#include "adc_library.h"
#include "pwm_library.h"

//3-part lib of lcd driver
#include "driver/lcdpcf8574.h"
#include "driver/pcf8574.h"
#include "driver/diagnostic.h"

#define F_CPU 16000000UL

//declare functions
void InitialiseGeneral();
void Initialise_Btn_INTs();
void InitialiseTimer1();
//void InitialiseTimer5();
void Initializse_LCD();
void initializse_mode_data();
void update_mode_data();
void update_led_state();
void update_channel_led();
void Enable_Btn_INTs();
void Disable_Btn_INTs();
void Update_LCD();
void trigger_mode_change_btn();
void trigger_setting_btn();
void trigger_start_stop_btn();
void change_adc_channel(unsigned char new_channel);

//macro for bit operation
#define setbit(port, bit) (port) |= (1 << (bit))	// Set Bit ON
#define clearbit(port, bit) (port) &= ~(1 << (bit))	// Set Bit Off
#define blinkbit(port, bit) (port) ^= (1 <<(bit))	// XOR bit

//define ADC channels 
#define CHANNEL0 0b01100000
#define CHANNEL1 0b01100001
#define CHANNEL2 0b01100010
#define CHANNEL3 0b01100011

#define SAMPLE_RATE_76K 76		//76KHz for ADC
#define FREQ_245HZ		0		//num for generate 245hz waveform 
#define FREQ_1KHZ		5		//num for generate 1Khz waveform 
#define FREQ_7_8KHZ		40		//num for generate 7.8Khz waveform 
#define FREQ_62_5KHZ	65		//num for generate 62.5Khz waveform 

#define BAUD_2M			2000000 //baud rate default for usart setup
#define BARU_96K		9600	//baud rate 9600;

// define LED port mapping
#define ADC_LED		DDC0	//PORTC0
#define RGB_LED_R	DDC1	//PORTC1
#define RGB_LED_G	DDC2	//PORTC2
#define RGB_LED_B	DDC3	//PORTC3

#define PWM_LED		DDC4	//PORTC4
// define H/W interrupts for buttons
#define START_STOP_BUTTON INT2_vect
#define MODE_BUTTON		  INT3_vect
#define SETTING_BUTTON	  INT4_vect

// default string length for mode information
#define STR_LEN 10	
// define a struct for store mode data
struct MODE_Info
{
	char mode_name[STR_LEN];			// static mode title 
	char label_SRorFN[STR_LEN];	// static SR or FN string
	char label_CHorPW[STR_LEN];	// static CH or PW string
	char mode_state[STR_LEN];	// start or stop state
	char data_chpw[STR_LEN];	// real time channel or pulse width data
	char data_srfn[STR_LEN];	// real time sample rate or freq data
	uint8_t freq;					//default 5kHz for PWM
	uint8_t pulse_width;			//default 1 for PWM
	uint8_t sample_rate;			//default 76kHz	for ADC
	uint8_t new_channel;		//default 0 for ADC
};

//declare two mode info struct var
struct MODE_Info adc_info;
struct MODE_Info pwm_info;
struct MODE_Info sleep_info;
//declare a struct pointer link to on going mode
static struct MODE_Info* mode_info = NULL;

//define two basic operation modes
typedef enum  {ADC_MODE, PWM_MODE, SLEEP_MODE}  OPT_MODE;
OPT_MODE cur_mode;

//define 2 actions for button 3 
typedef enum {STOP, START} BTN_ACTION;			
BTN_ACTION goto_action = START;

uint8_t channel_count = 0;
uint8_t freq_count = 0;

//#define TIMING_DIAGNOS  1 // comment this line when no using time diagnostic functions

int main(void)
{
	InitialiseGeneral();

#ifdef TIMING_DIAGNOS
	Initialise_Usart(BARU_96K);	
	Initialise_TimingDiagnostic();	// for timing diagnostic
	g_iCheckPointArrayIndex = 0;	// for timing diagnostic
#else
	Initialise_Usart(BAUD_2M);
#endif

#ifdef TIMING_DIAGNOS
	TimingDiagnostic_ResetTimer4();
	TimingDiagnostic_CheckPoint(g_iCheckPointArrayIndex++); // timing diagnostic 
	TimingDiagnostic_CheckPoint(g_iCheckPointArrayIndex++); // timing diagnostic --check the offset
	TimingDiagnostic_ResetTimer4();		
	initializse_mode_data();
	TimingDiagnostic_ResetTimer4();
	InitialiseTimer1();
	Initialise_Btn_INTs();
	TimingDiagnostic_ResetTimer4();
	TimingDiagnostic_Display_CheckPoint_DataViaUSART0(g_iCheckPointArrayIndex);
#else
	initializse_mode_data();
	InitialiseTimer1();
	Initialise_Btn_INTs();
#endif

	Initializse_LCD(); 
	Enable_Btn_INTs();
	setSamplerateADC(SAMPLE_RATE_76K);
	Initialise_ADC();
	startADC();	
	
	while (1)
	{
		Update_LCD(); 
	}
}

void initializse_mode_data()
{
#ifdef TIMING_DIAGNOS
	TimingDiagnostic_ResetTimer4();
	TimingDiagnostic_CheckPoint(g_iCheckPointArrayIndex++); // timing diagnostic
#endif

	// initial default mode information
	strcpy(adc_info.mode_name,"ADC_MODE ");
	strcpy(adc_info.label_SRorFN,"SR:");
	strcpy(adc_info.label_CHorPW,"CH:");
	strcpy(adc_info.mode_state,"stop");
	strcpy(adc_info.data_srfn,"76K  ");
	strcpy(adc_info.data_chpw,"1  ");
	adc_info.new_channel = CHANNEL1;	//default A0 channel
	adc_info.sample_rate = SAMPLE_RATE_76K;	//default 76kHz
	channel_count = 2;

	strcpy(pwm_info.mode_name,"PWM_MODE ");
	strcpy(pwm_info.label_SRorFN,"FN:");
	strcpy(pwm_info.label_CHorPW,"PW:");
	strcpy(adc_info.mode_state,"stop");
	strcpy(pwm_info.data_srfn,"62.5K");
	strcpy(pwm_info.data_chpw,"20 ");
	pwm_info.freq = FREQ_62_5KHZ;
	pwm_info.pulse_width = 1;

	strcpy(sleep_info.mode_name,"SLEEP_MODE");
	strcpy(sleep_info.mode_state,"ON");

	cur_mode = ADC_MODE;    //default start mode
	mode_info = &adc_info;	//pointer to default mode - ADC mode

#ifdef TIMING_DIAGNOS
	TimingDiagnostic_CheckPoint(g_iCheckPointArrayIndex++); // timing diagnostic
#endif
}
void Initializse_LCD()
{
	lcd_init(LCD_DISP_ON);	// initial lcd driver
	lcd_home();
	lcd_led(0);
}

void Update_LCD()
{
	if (cur_mode != SLEEP_MODE)
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
		lcd_puts("  ");
	}
	else
	{
		lcd_gotoxy(0, 0);
		lcd_puts (mode_info->mode_name);
		lcd_puts ("    ");
		lcd_gotoxy(0, 1);
		lcd_puts ("                 "); // clean the lcd
	}
}

void InitialiseGeneral()
{
	//Port for connecting switches
	DDRB = 0x00;			// Configure PortB direction for Output
	PORTB = 0x00;			
	//Port for connecting output LED
	DDRC = 0xFF;
	PORTC = 0x00;	

	sei();	// set Global Interrupt Enable (I) bit
}

void InitialiseTimer1()		// Configure to generate an interrupt after a 1-Second interval
{
#ifdef TIMING_DIAGNOS
TimingDiagnostic_ResetTimer4();
TimingDiagnostic_CheckPoint(g_iCheckPointArrayIndex++); // timing diagnostic
#endif
	TCCR1A = 0b00000000;	// Clear Timer on 'Compare Match' (CTC) waveform mode)
	TCCR1B = 0b00001100;	// CTC waveform mode, use prescaler 256
	TCCR1C = 0b00000000;
	
	// For 16 MHz clock (with 256 prescaler) to achieve a 1 second interval: 16M/256 = 0xF424 (62,500)
	// to achive 0.5 second interval: 16M/256 /2 = 0x7A12 (decimal 31,250)
	OCR1AH = 0x7A;
	OCR1AL = 0x12;// 0.5 second

	TCNT1H = 0b00000000;	// Timer/Counter count/value registers (16 bit) TCNT1H and TCNT1L
	TCNT1L = 0b00000000;
	TIMSK1 = 0b00000010;	// bit 1 OCIE1A		Use 'Output Compare A Match' Interrupt, 
#ifdef TIMING_DIAGNOS
TimingDiagnostic_CheckPoint(g_iCheckPointArrayIndex++); // timing diagnostic
#endif
}

void update_channel_led()
{	
	unsigned char cur_channel = adc_info.new_channel;
	switch(cur_channel)
	{
		case CHANNEL0:
			setbit(PORTC,RGB_LED_R);	// Lit ON RGB - Red
			setbit(PORTC,RGB_LED_G);	// Lit ON RGB - Green
			setbit(PORTC,RGB_LED_B);	// Lit ON RGB - Blue
		break;
		case CHANNEL1:
			setbit(PORTC,RGB_LED_G);	// Lit ON RGB - Green
			clearbit(PORTC,RGB_LED_R);	// Lit Off Red
			clearbit(PORTC,RGB_LED_B);  // Lit off RGB 
		break;
		case CHANNEL2: 
			setbit(PORTC,RGB_LED_B);	// Lit ON RGB - Blue
			clearbit(PORTC,RGB_LED_G);	// Lit Off Green
			clearbit(PORTC,RGB_LED_R);	// Lit Off Red
		break;
		case CHANNEL3:
			setbit(PORTC,RGB_LED_R);	// Lit ON RGB - Red
			clearbit(PORTC,RGB_LED_G);	// Lit Off Green
			clearbit(PORTC,RGB_LED_B); // Lit Off Blue
		break;
	}
}

void update_led_state()
{
	OPT_MODE temp = cur_mode;
	switch(temp)
	{
		case ADC_MODE:
			if (goto_action == START) setbit(PORTC,ADC_LED);
			else blinkbit(PORTC,ADC_LED);
			clearbit(PORTC,PWM_LED); // lit off PWM LED
			update_channel_led();
		break;
		case PWM_MODE:
			if (goto_action == START) setbit(PORTC,PWM_LED);
			else blinkbit(PORTC,PWM_LED);
			//lit off all ADC and Channel LEDs
			clearbit(PORTC,ADC_LED);
			clearbit(PORTC,RGB_LED_R);
			clearbit(PORTC,RGB_LED_G);
			clearbit(PORTC,RGB_LED_B);
		break;
		case SLEEP_MODE: //lit off all led
			clearbit(PORTC,PWM_LED);
			clearbit(PORTC,ADC_LED);
			clearbit(PORTC,RGB_LED_R);
			clearbit(PORTC,RGB_LED_G);
			clearbit(PORTC,RGB_LED_B);
		break;
	}
}


void update_mode_data() 
{	
	uint8_t temp = ADCH;
	switch(cur_mode)
	{
		case ADC_MODE:
			if (temp < 50){
				adc_info.sample_rate = 9;
				strcpy(adc_info.data_srfn," 9K  ");
			}
			else if (temp < 100){
				adc_info.sample_rate = 19;
				strcpy(adc_info.data_srfn,"19K  ");
			}
			else if (temp < 150){
				adc_info.sample_rate = 38;
				strcpy(adc_info.data_srfn,"38K  ");
			}
			else if (temp < 200){
				adc_info.sample_rate = 76;
				strcpy(adc_info.data_srfn,"76K  ");
			}
			else{
				adc_info.sample_rate = 152;	//152K may cause some problem
				strcpy(adc_info.data_srfn,"152K ");
			}
		break;

		case PWM_MODE:
			pwm_info.pulse_width = (int)temp;
			itoa(temp, pwm_info.data_chpw, STR_LEN);
		break;
		case SLEEP_MODE:
			// do some things here for sleep mode in future		
		break;
	}	
}

ISR(TIMER1_COMPA_vect) // TIMER1_CompareA_Handler (Interrupt Handler for Timer 1)
{
	if (goto_action == START) update_mode_data();
	update_led_state();
}

ISR(ADC_vect)	// ADC Interrupt Handler
{
	#ifndef TIMING_DIAGNOS
    USART_TX_SingleByte(ADCH);
	#endif
}

// Initialize 3 INT (2,3,4) for Button events
void Initialise_Btn_INTs()
{
#ifdef TIMING_DIAGNOS
TimingDiagnostic_ResetTimer4();
TimingDiagnostic_CheckPoint(g_iCheckPointArrayIndex++); // timing diagnostic
#endif
	EICRA = 0b10100000;	// falling-edge triggered for Button 1 & 2	
	EICRB = 0b00000010; // falling-edge triggered for Button 3	
	EIMSK = 0b00000000;	// Initially disabled, 		
	EIFR = 0b11111111;	// Clear all HW interrupt flags 
#ifdef TIMING_DIAGNOS
TimingDiagnostic_CheckPoint(g_iCheckPointArrayIndex++); // timing diagnostic
#endif
}

void Enable_Btn_INTs()
{
	EIMSK = 0b00011100;		// Enable H/W Int 2,3,4 for Button 1, 2, 3
}

void Disable_Btn_INTs()
{
	EIMSK = 0b00000000;		// Disable H/W Int 
}

//Setting Button for user changing channels of ADC or frequencies of PWM
ISR(SETTING_BUTTON)
{
	Disable_Btn_INTs();	// Disable INT to prevent key bounce
	if (cur_mode==SLEEP_MODE)
		trigger_mode_change_btn();
	else if(goto_action == START)
		trigger_setting_btn();
	Enable_Btn_INTs();	// Re-enable the interrupt
}

// Start/Stop Button Int for start mode settings or stop to waiting for user input
ISR(START_STOP_BUTTON) // Interrupt Handler for H/W INT 2
{
	Disable_Btn_INTs();		// Disable INT to prevent key bounce
	if (cur_mode == SLEEP_MODE)
		trigger_mode_change_btn();
	else
		trigger_start_stop_btn();
	Enable_Btn_INTs();		// Re-enable the interrupt
}

// Mode Button Int for switch among 3 modes (ADC, PWM, SLEEP) 
ISR(MODE_BUTTON)  
{
	Disable_Btn_INTs();
	if (goto_action == START)
		trigger_mode_change_btn();	
	Enable_Btn_INTs();
} 

void trigger_mode_change_btn()
{
	OPT_MODE temp = cur_mode;
	switch(temp)
	{
		case ADC_MODE:
			cur_mode = PWM_MODE;
			change_adc_channel(CHANNEL0);	// ch0 for potentiometer 
			mode_info = &pwm_info;
		break;
		case PWM_MODE:
			cur_mode = SLEEP_MODE;
			mode_info = &sleep_info;
		break;
		case SLEEP_MODE:
			cur_mode = ADC_MODE;
			mode_info = &adc_info;
		break;
	}
}

void trigger_setting_btn()
{
	OPT_MODE temp = cur_mode;
	if (temp == PWM_MODE)
	{
		switch(freq_count)
		{
			case 0:
				pwm_info.freq = FREQ_245HZ;
				strncpy(pwm_info.data_srfn,"245Hz", STR_LEN);
			break;
			case 1:
				pwm_info.freq = FREQ_1KHZ;
				strncpy(pwm_info.data_srfn,"1KHz ", STR_LEN);
			break;
			case 2:
				pwm_info.freq = FREQ_7_8KHZ;
				strncpy(pwm_info.data_srfn,"7.8Kh", STR_LEN);
			break;
			case 3:
				pwm_info.freq = FREQ_62_5KHZ;
				strncpy(pwm_info.data_srfn,"62.5K", STR_LEN);
			break;
		}

		freq_count++;

		if (freq_count > 3)
			freq_count = 0;
	}
	else if(temp == ADC_MODE)
	{
		switch(channel_count)
		{
			case 0:
				adc_info.new_channel = CHANNEL0;
				strncpy(adc_info.data_chpw,"0  ", STR_LEN);
			break;
			case 1:
				adc_info.new_channel = CHANNEL1;
				strncpy(adc_info.data_chpw,"1  ", STR_LEN);
			break;
			case 2:
				adc_info.new_channel = CHANNEL2;
				strncpy(adc_info.data_chpw,"2  ", STR_LEN);
			break;
			case 3:
				adc_info.new_channel = CHANNEL3;
				strncpy(adc_info.data_chpw,"3  ", STR_LEN);
			break;
		}
		channel_count++;

		if (channel_count > 3)
		channel_count = 1;
	}
}

void change_adc_channel(unsigned char new_channel)
{
	ADMUX = new_channel;
}

void trigger_start_stop_btn()
{
	BTN_ACTION cur_action = goto_action;
	switch(cur_action)
	{
		case STOP:
			change_adc_channel(CHANNEL0); // channel0 for potentiometer
			strcpy(adc_info.mode_state,"stop  ");
			strcpy(pwm_info.mode_state,"stop  ");
			goto_action = START;	// next action		
		break;
		case START:
			if (cur_mode == ADC_MODE)
			{
				change_adc_channel(adc_info.new_channel);
				setSamplerateADC(adc_info.sample_rate);
				strcpy(adc_info.mode_state,"start");
			}
			else if (cur_mode == PWM_LED)  //in PWM mode
			{
				strcpy(pwm_info.mode_state,"start");
				enablePWM(0,pwm_info.pulse_width,pwm_info.freq);
			}
			else // in sleep mode
			{
				// in future do something
			}
			goto_action = STOP;		//next action
		break;
	}
}
// Button 2 event for setting mode parameters 

