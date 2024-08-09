/*
 * main.c
 *
 * Created: 8/9/2024 11:07:13 AM
 *  Author: Luis Angel Velasquez
 */ 

#define F_CPU 16000000
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "DS18B20/DS18B20.h"

#define DS18B20_PIN  PB0  // Define el pin al que está conectado el DS18B20

// Configuración de UART
void uart_init(unsigned long baud) {
	unsigned int ubrr = F_CPU / 16 / baud - 1;
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void uart_print(const char *str) {
	while (*str) {
		uart_transmit(*str++);
	}
}

int main(void) {
	char buffer[16];
	int16_t temperature;
	
	// Configura el pin del DS18B20
	DDRB |= (1 << DS18B20_PIN);  // Configura el pin como salida
	PORTB |= (1 << DS18B20_PIN); 

	// Configura UART
	uart_init(9600);  // Configura la UART a 9600 baudios

	while (1) {
		
		temperature = readTemperature();
		
		// Convierte la temperatura a cadena de caracteres
		snprintf(buffer, sizeof(buffer), "Temp: %d.%d C\r\n", temperature / 16, (temperature % 16) * 100 / 16);
		
		// Envía la cadena a través de UART
		uart_print(buffer);
		
		_delay_ms(1000); // Espera 1 segundo antes de la siguiente lectura
	}

	return 0;
}
