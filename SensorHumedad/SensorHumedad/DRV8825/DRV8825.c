/*
 * DRV8825.c
 *
 * Created: 22/08/2024 20:32:08
 *  Author: Luis Angel Velasquez
 */ 


#include "DRV8825.h"

#define DIR_PIN PC1
#define STEP_PIN PC2
#define STEPS_PER_REVOLUTION 200
#define velocidad 600

void setup_pins() {
	// Configurar los pines como salidas
	DDRC |= (1 << DIR_PIN) | (1 << STEP_PIN);
}

void step_motor(void) {
	// Generar un pulso de paso
	PORTC |= (1 << STEP_PIN);
	_delay_us(1000);
	PORTC &= ~(1 << STEP_PIN);
	_delay_us(1000);
}

void rotate_motor(uint8_t direction, uint16_t steps) {
	// Configurar la dirección del motor
	if (direction == 1) {
		PORTC |= (1 << DIR_PIN);  // Sentido horario
		} else {
		PORTC &= ~(1 << DIR_PIN); // Sentido antihorario
	}

	// Ejecutar los pasos
	for (uint16_t i = 0; i < steps; i++) {
		step_motor();
	}
}

//Ejemplos de uso:
	// Rotar 1 revolución en sentido horario lentamente
	//rotate_motor(1, 1*STEPS_PER_REVOLUTION);
	
	// Rotar 1 revolución en sentido antihorario rápidamente
	//rotate_motor(0, 0.5*STEPS_PER_REVOLUTION);

