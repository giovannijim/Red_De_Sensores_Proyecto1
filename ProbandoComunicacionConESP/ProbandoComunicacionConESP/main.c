/*
 * ProbandoComunicacionConESP.c
 *
 * Created: 8/15/2024 6:10:23 PM
 * Author : alesa
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "UART/UART.h"

uint8_t BufferRx;


int main(void)
{
    cli();
	DDRB |= (1<<DDB3);
	DDRB |= (1<<DDB3);
	PORTB = 0;
	initUART9600();
	sei();
    while (1) 
    {
		//writeUART('a');
		
		_delay_ms(3000);
    }
}


ISR(USART_RX_vect)
{
	BufferRx = UDR0;
	if (UDR0 == '1')
	{
		PORTB |= 1<<PORTB3;
	}
	else{
		PINB |= 1<<PINB4;
	}

}