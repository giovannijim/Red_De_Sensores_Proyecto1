/*
 * SensorHumedad.c
 * Created: 8/9/2024 11:46:15 AM
 * Author : Giovanni Jimenez
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "ADC/initADC.h"
#include "UART/UART.h"
#include "I2C/TWI_I2C.h"

void float_to_char_s2(char value_adch);

#define SlaveHumedad 0x30

uint8_t buffer = 0;
char buffer_s2[5] = {'0'};
uint8_t	 moisture_level;
uint8_t bufferRX;
float voltage;

int main(void)
{
	cli();
    moisture_level = 0;
	initUART9600();
	I2C_SETTING_SLAVE(SlaveHumedad);
	sei();
    while (1) 
    {
		initADC(0);							// Inicializar ADC [0]
		ADCSRA |= (1<< ADSC);				// Comenzar conversion
		while(ADCSRA&(1<<ADSC));			// Revisar si la conversion ya termino
		moisture_level = ~ADCH;
		float_to_char_s2(moisture_level);
		_delay_ms(20);
    }
}

// Subrutina para convertir un valor float a un buffer char
void float_to_char_s2(char value_adch){
	int a, b;									// Se designa el tipo de variable de a y b
	voltage = value_adch * 0.01960784314;		// Se realizar una regla de 3 para escalar [0-255] a [0.00-5.00]
	
	a = (int)voltage;							// Se obtienen las cifras enteras de la variable float
	b = ((int)(voltage*1000))%1000;				// Se obtienen las cifras decimales de la variable float
	
	snprintf(buffer_s2, 5, "%d.%2.2d", a, b);		// Se convierte une todo en una cadena char
}

// Vector de interrupcion ADC 
ISR(ADC_vect)
{
	// Se escribe con un 1 lógico la bandera para apagarla
	ADCSRA |= (1<<ADIF);
}

ISR(USART_RX_vect)
{
	// Escribir en la terminal el valor de cada uno de los potenciometros
	writeUART('\n');
	cadena("Valor de humedad:");
	cadena(buffer_s2);
	//Se almacena en la variable lo que se recibe de UDR0
	bufferRX = UDR0;
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
		buffer = TWDR;
		TWCR |= (1<<TWINT); //Se limpia la bandera
		break;
		case 0xA8: // SLA+R recibido, maestro solicita lectura
		case 0xB8: // Dato transmitido y ACK recibido
		TWDR = moisture_level; // Cargar el dato en el registro de datos*****************
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
