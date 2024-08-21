/*
 * DS18B20.c
 *
 * Created: 9/08/2024 12:09:46
 *  Author: Luis Angel Velasquez
 */ 

#define F_CPU 16000000
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
// Definiciones para el DS18B20
#define DS18B20_PIN  PB0  // Define el pin al que está conectado el DS18B20

// Funciones para el DS18B20
void oneWireInit(void) {
	DDRB |= (1 << DS18B20_PIN);  // Configura el pin como salida
	PORTB &= ~(1 << DS18B20_PIN); // Envía un pulso bajo
	_delay_us(480);  // Espera 480us
	PORTB |= (1 << DS18B20_PIN);  // Envía un pulso alto
	_delay_us(70);   // Espera 70us
	DDRB &= ~(1 << DS18B20_PIN); // Configura el pin como entrada
	_delay_us(410);  // Espera 410us para la respuesta del sensor
}

void oneWireWrite(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		DDRB |= (1 << DS18B20_PIN);  // Configura el pin como salida
		PORTB &= ~(1 << DS18B20_PIN); // Envía un pulso bajo
		_delay_us(2);  // Espera 2us
		if (data & 0x01) {
			PORTB |= (1 << DS18B20_PIN); // Envía un pulso alto
		}
		_delay_us(60);  // Espera 60us
		PORTB |= (1 << DS18B20_PIN);  // Termina el pulso
		_delay_us(2);   // Espera 2us
		data >>= 1;    // Desplaza el bit
	}
}

uint8_t oneWireRead(void) {
	uint8_t data = 0;
	for (uint8_t i = 0; i < 8; i++) {
		DDRB |= (1 << DS18B20_PIN);  // Configura el pin como salida
		PORTB &= ~(1 << DS18B20_PIN); // Envía un pulso bajo
		_delay_us(2);  // Espera 2us
		DDRB &= ~(1 << DS18B20_PIN); // Configura el pin como entrada
		_delay_us(10);  // Espera 10us para leer el bit
		if (PINB & (1 << DS18B20_PIN)) {
			data |= (1 << i); // Lee el bit
		}
		_delay_us(55);  // Espera 55us
	}
	return data;
}

uint8_t readTemperature(void) {
	 uint16_t temperatureRaw;
	 uint8_t temperatureCelsius;

	
	// Inicialización y lectura de la temperatura
	oneWireInit();
	oneWireWrite(0xCC); // Skip ROM command
	oneWireWrite(0x44); // Convert T command
	_delay_ms(750);     // Espera a que el sensor convierta la temperatura
	
	oneWireInit();
	oneWireWrite(0xCC); // Skip ROM command
	oneWireWrite(0xBE); // Read Scratchpad command
	
	uint8_t tempL = oneWireRead();
	uint8_t tempH = oneWireRead();
	
	temperatureRaw = (tempH << 8) | tempL; // Combina los bytes de temperatura

	 // Conversión a grados Celsius. El valor del DS18B20 es en unidades de 1/16 de grado.
	 temperatureCelsius = temperatureRaw * 0.0625;  // Ahora es un float en °C
/*
	 // Ajuste a un rango de 0-255, si quieres una precisión de 1°C por bit
	 uint8_t temperatureScaled = (uint8_t)(temperatureCelsius); // Redondeo correcto

	 if (temperatureScaled > 255) {
		 temperatureScaled = 255;  // Limitar a 255 si excede
	 }
*/
	 return temperatureCelsius;  // Devuelve la temperatura escalada
}