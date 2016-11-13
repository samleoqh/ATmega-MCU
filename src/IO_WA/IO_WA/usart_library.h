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
void USART_SETUP_BAUD_ASSUME_16MHz_CLOCK(int32_t baudrate);

#endif //__USART_LIBRARY_H__
