/*
* usart_library.cpp
*
* Created: 16-10-2016 20:49:13
* Author: ravim
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart_library.h"



//**************USART ****************************//

void USART_SETUP_9600_BAUD_ASSUME_1MHz_CLOCK()
{
	// UCSR0A ?USART Control and Status Register A
	// bit 1 UX2 Double the USART TX speed (also depends Baud Rate Registers)
	UCSR0A = 0b00000010;   // Set U2X (Double USART Tx speed, to reduce clocking error)

	// UCSR0B - USART Control and Status Register B
	// bit 7 RXCIE Receive Complete Interrupt Enable
	// bit 6 TXCIE Transmit Complete Interrupt Enable
	// bit 5 UDRIE Data Register Empty Interrupt Enable
	// bit 4 RXEN Receiver Enable
	// bit 3 TXEN Transmitter Enable
	// bit 2 UCSZ2 Character Size (0 = 5,6,7 or 8-bit data,  1 = 9-bit data)
	UCSR0B = 0b10011000;   // RX Complete Int Enable, RX Enable, TX Enable, 8-bit data

	// UCSR0C - USART Control and Status Register C
	// *** This register shares the same I/O location as UBRRH ***
	// Bits 7:6 ?UMSELn1:0 USART Mode Select (00 = Asynchronous)
	// bit 5:4 UPM1:0 Parity Mode
	// bit 3 USBS Stop Bit Select
	// bit 2:1 UCSZ1:0 Character Size (see also UCSZ2 in UCSRB)
	// bit 0 UCPOL Clock Polarity
	UCSR0C = 0b00000111;   // Asynchronous, No Parity, 1 stop, 8-bit data, Falling XCK edge

	// UBRR0 - USART0 Baud Rate Register (16-bit register, comprising UBRR0H and UBRR0L)
	UBRR0H = 0;   // 9600 baud, UBRR = 12, and  U2X must be set to '1' in UCSRA
	UBRR0L = 12;
}



void USART_TX_SingleByte (unsigned char cByte)
{
	while( ! ( UCSR0A  &  ( 1 << UDRE0 ) ) );
	// Wait for Tx buffer to be empty (check UDRE flag)
	
	UDR0 = cByte; 	// Writing to the UDR Tx buffer transmits the byte
}
