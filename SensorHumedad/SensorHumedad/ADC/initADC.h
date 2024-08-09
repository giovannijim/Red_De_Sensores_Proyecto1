/*
 * initADC.h
 * Created: 7/19/2024 7:53:41 AM
 *  Author: Giovanni
 */ 


#ifndef INITADC_H_
#define INITADC_H_

#include <avr/io.h>
#include <stdint.h>

// Funcion para indicar que puerto ADC desea activar
void initADC(uint8_t puerto);

#endif /* INITADC_H_ */