/*
 * DS18B20.h
 *
 * Created: 9/08/2024 12:10:07
 *  Author: Luis Angel Velasquez
 */ 


#ifndef DS18B20_H_
#define DS18B20_H_

void oneWireInit(void);
void oneWireWrite(uint8_t data);
uint8_t oneWireRead(void);
int16_t readTemperature(void);




#endif /* DS18B20_H_ */