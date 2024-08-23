/*
 * main.c
 * Created: 8/9/2024 11:07:13 AM
 *  Author: Luis Angel Velasquez
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "DS18B20/DS18B20.h"
#include "I2C/TWI_I2C.h"
#include "PWM0/PMW0.h"

#define SlaveTemp 0x40
#define DS18B20_PIN  PB0  // Define el pin al que está conectado el DS18B20
#define temp_sup 26
#define temp_infe 23

uint8_t bufferI2C = 0;
uint8_t temperatureRaw;
uint8_t bandera_ada=0;
/*
void uart_init(unsigned long baud);
void uart_transmit(unsigned char data);
void uart_print(const char *str);
*/

void abrir_compuertas(void){
	duty_cycleA(32);
	duty_cycleB(32);
}

void cerrar_compuertas(void){
	duty_cycleA(16);
	duty_cycleB(19);
	
}
int main(void) {
	
	//Incialización de los PWM0 
	init_PMW0A(0,0,1024);
	init_PMW0B(0);
	//Posición inicial
	cerrar_compuertas();
	
	cli();
	//char buffer[16];
	DDRB |= 1<< DDB4;
	PORTB &= ~1<<PORTB4;
	
	// Configura el pin del DS18B20
	DDRB |= (1 << DS18B20_PIN);  // Configura el pin como salida
	PORTB |= (1 << DS18B20_PIN); 

	// Configura UART
	//uart_init(9600);  // Configura la UART a 9600 baudios
	I2C_SETTING_SLAVE(SlaveTemp);
	sei();
	while (1) {
		
		temperatureRaw = readTemperature();
		
		//Condiciones para cerrar y abrir compuertas
		if (temperatureRaw > temp_sup) {
			abrir_compuertas();
		}
		if (temperatureRaw < temp_infe){
			cerrar_compuertas();
		}
		
		if (bufferI2C == 1)
		{
			PORTB |= 1<< PORTB4;
			abrir_compuertas();
		}
		else if (bufferI2C == 2) {
			PORTB &= ~1<< PORTB4;
			cerrar_compuertas();
		}
		
		
		// Convierte la temperatura a cadena de caracteres
		//snprintf(buffer, sizeof(buffer), "%d.%02d C\r\n", temperatureRaw, (temperatureRaw) * 100 / 16);
		
		// Envía la cadena a través de UART
		//uart_print(buffer);
		
		
		_delay_ms(20); // Espera 1 segundo antes de la siguiente lectura
	}

}

// Vector de interrupcion I2C
ISR(TWI_vect){
	uint8_t estado;
	estado = TWSR & 0xFC;
	switch(estado){
		case 0x60:
		case 0x70:
		TWCR |= (1<<TWINT);
		break;
		case 0x80:
		case 0x90:
		bufferI2C = TWDR;
		TWCR |= (1<<TWINT); //Se limpia la bandera
		break;
		case 0xA8: // SLA+R recibido, maestro solicita lectura
		case 0xB8: // Dato transmitido y ACK recibido
		TWDR = temperatureRaw; // Cargar el dato en el registro de datos*******
		TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWIE)| (1 << TWEA); // Listo para la proxima operacion
		break;
		case 0xC0: // Dato transmitido y NACK recibido
		case 0xC8: // Ultimo dato transmitido y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Listo para la proxima operacion
		break;
		default:	//Se libera el bus de cualquier error
		TWCR |= (1<<TWINT)|(1<<TWSTO);
		break;
	}
}

/*
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
*/

// Vector de interrupcion UART
/*
ISR(USART_RX_vect)
{
	uint8_t BufferRx = UDR0;
}
*/