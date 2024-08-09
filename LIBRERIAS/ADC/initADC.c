/*
 * initADC.c
 * Created: 7/19/2024 7:54:20 AM
 *  Author: Giovanni
 */ 

#include <avr/io.h>
#include <stdint.h>

void initADC(uint8_t puertoADC){
	ADMUX = 0;
	// Se selecciona un canal
	ADMUX = puertoADC;
	
	// Se selecciona el voltaje VREF 5V
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	// Se justifica hacia la izquierda la salida del adc
	ADMUX |= (1 << ADLAR);
	// Habilitar prescaler de 16M/128 fadc = 125khz
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	// Activar la interrupcion del ADC
	ADCSRA |= (1<<ADIE);
	// Se activa el ADC
	ADCSRA |= (1<< ADEN);
}