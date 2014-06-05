
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

#ifndef USART_H
#define USART_H
#include <avr/io.h>
// USART Setup Values
#define F_CPU 8000000UL // Assume uC operates at 8MHz
#define BAUD_RATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD_RATE * 16UL))) - 1)

////////////////////////////////////////////////////////////////////////////////
//Functionality - Initializes TX and RX on PORT D
//Parameter: None
//Returns: None
void initUSART0()
{
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_RATE;
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	UCSR0C |= (1 << USBS0) | (1 << UCSZ00) | (1 << UCSZ01);
}
void initUSART1()
{
	UBRR1H = (BAUD_PRESCALE >> 8);
	UBRR1L = BAUD_RATE;
	UCSR1B |= (1 << RXEN1) | (1 << TXEN1);
	UCSR1C |= (1 << USBS1) | (1 << UCSZ10) | (1 << UCSZ11);
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - checks if USART is ready to send
//Parameter: None
//Returns: 1 if true else 0
unsigned char USART_IsSendReady0()
{
	return (UCSR0A & (1 << UDRE0));
}
unsigned char USART_IsSendReady1()
{
	return (UCSR1A & (1 << UDRE1));
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - checks if USART has recieved data
//Parameter: None
//Returns: 1 if true else 0
unsigned char USART_HasTransmitted0()
{
	return (UCSR0A & (1 << TXC0));
}
unsigned char USART_HasTransmitted1()
{
	return (UCSR1A & (1 << TXC1));
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - checks if USART has recieved data
//Parameter: None
//Returns: 1 if true else 0
unsigned char USART_HasReceived0()
{
	return (UCSR0A & (1 << RXC0));
}
unsigned char USART_HasReceived1()
{
	return (UCSR1A & (1 << RXC1));
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Flushes the data register
//Parameter: None
//Returns: None
void USART_Flush0()
{
	static unsigned char dummy;
	while ( UCSR0A & (1 << RXC0) ) { dummy = UDR0; }
}
void USART_Flush1()
{
	static unsigned char dummy;
	while ( UCSR1A & (1 << RXC1) ) { dummy = UDR1; }
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - Sends an 8-bit char value
//Parameter: Takes a single unsigned char value
//Returns: None
void USART_Send0(unsigned char sendMe)
{
	while( !(UCSR0A & (1 << UDRE0)) );
	UDR0 = sendMe;
}
void USART_Send1(unsigned char sendMe)
{
	while( !(UCSR1A & (1 << UDRE1)) );
	UDR1 = sendMe;
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - receives an 8-bit char value
//Parameter: None
//Returns: Unsigned char data from the receive buffer
unsigned char USART_Receive0()
{
	while ( !(UCSR0A & (1 << RXC0)) ); // Wait for data to be received
	return UDR0; // Get and return received data from buffer
}
unsigned char USART_Receive1()
{
	while ( !(UCSR1A & (1 << RXC1)) ); // Wait for data to be received
	return UDR1; // Get and return received data from buffer
}
#endif //USART_H