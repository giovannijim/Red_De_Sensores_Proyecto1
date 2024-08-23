/*
 * DRV8825.h
 *
 * Created: 22/08/2024 20:32:22
 *  Author: Luis Angel Velasquez
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

#ifndef DRV8825_H_
#define DRV8825_H_

void rotate_motor(uint8_t direction, uint16_t steps);
void step_motor(void);
void setup_pins();



#endif /* DRV8825_H_ */