/*
 * pwm_library.h
 *
 * Created: 17-11-2016 12:23:20
 *  Author: ravim
 */ 
 #include <avr/io.h>
 

#ifndef PWM_LIBRARY_H_
#define PWM_LIBRARY_H_


void enablePWM();
void enableInvertingPWM(int pwm);
void changePWMDutyCycle(int pwm,int pulseWidth);
void enablePWM(int number , int pulseWidth, int frequency);


#endif /* PWM_LIBRARY_H_ */