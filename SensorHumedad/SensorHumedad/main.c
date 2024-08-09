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

void float_to_char_s2(char value_adch);

char buffer_s2[5] = {'0'};
uint8_t	 moisture_level;
uint8_t bufferRX;
float voltage;

int main(void)
{
    moisture_level = 0;
	initUART9600();
	sei();
    while (1) 
    {
		initADC(0);							// Inicializar ADC [0]
		ADCSRA |= (1<< ADSC);				// Comenzar conversion
		while(ADCSRA&(1<<ADSC));			// Revisar si la conversion ya termino
		moisture_level = ADCH;
		float_to_char_s2(moisture_level);
		_delay_ms(20);
    }
}

void float_to_char_s2(char value_adch){
	int a, b;									// Se designa el tipo de variable de a y b
	voltage = value_adch * 0.01960784314;		// Se realizar una regla de 3 para escalar [0-255] a [0.00-5.00]
	
	a = (int)voltage;							// Se obtienen las cifras enteras de la variable float
	b = ((int)(voltage*1000))%1000;				// Se obtienen las cifras decimales de la variable float
	
	snprintf(buffer_s2, 5, "%d.%2.2d", a, b);		// Se convierte une todo en una cadena char
}

// Vector de interrupcion ADC -------------------------------------------------
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
