/*
 * TWI_I2C.h
 * Created: 8/2/2024 8:10:59 PM
 *  Author: alesa
 */ 


#ifndef TWI_I2C_H_
#define TWI_I2C_H_

#include <avr/io.h>
#include <stdbool.h>
#define F_CPU 16000000
#include <util/twi.h> // Biblioteca para definir los códigos de estado I2C

void I2C_SETTING_MASTER(uint8_t Prescaler, unsigned long SCL_Clock);
void I2C_SETTING_SLAVE(uint8_t address);
void I2C_SLAVE(uint8_t dato, uint8_t address);  //Dato, direccion del esclavo
uint8_t I2C_INIT();
uint8_t I2C_WRITE(uint8_t dato);
void I2C_STOP();

uint8_t I2C_READ(uint8_t address, uint8_t *dato);

#endif /* TWI_I2C_H_ */