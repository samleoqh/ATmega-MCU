/*
* usart_library.h
*
* Created: 16-10-2016 20:49:13
* Author: ravim
*/

#ifndef __USART_LIBRARY_H__
#define __USART_LIBRARY_H__

void USART_SETUP_9600_BAUD_ASSUME_1MHz_CLOCK();
void USART_TX_SingleByte (unsigned char cByte);
void Initialise_Usart(int32_t baudrate);

void USART0_TX_SingleByte(unsigned char cByte);
void USART0_TX_String(char* sData);
void USART0_DisplayBanner_Head();
void USART0_DisplayBanner_Tail();

#endif //__USART_LIBRARY_H__
