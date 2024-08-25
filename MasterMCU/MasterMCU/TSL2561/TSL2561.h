/*
 * TSL2561.h
 *
 * Created: 24/08/2024 12:26:39
 *  Author: Luis Angel Velasquez
 */ 


#ifndef TSL2561_H_
#define TSL2561_H_

#include <stdio.h>
#include <avr/io.h>
#include "I2C/TWI_I2C.h"

void TSL2561_Write(uint8_t reg, uint8_t value);
uint16_t TSL2561_read();
void TSL2561_power_on(void);
void TSL2561_set_timing(uint8_t gain, uint8_t integration_time);
uint8_t TSL2561_read_reg(uint8_t reg);




#endif /* TSL2561_H_ */