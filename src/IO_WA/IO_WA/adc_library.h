/*
* adc_library.h
*
* Created: 16-10-2016 20:41:18
* Author: ravim
*/


#ifndef __ADC_LIBRARY_H__
#define __ADC_LIBRARY_H__

void Initialise_ADC();
//void Start_ADC_Conversion(void);
void setSamplerateADC(int rate);
void startADC();
void stopADC();

#endif //__ADC_LIBRARY_H__
