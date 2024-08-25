/*
 * PMW0.h
 *
 * Created: 13/04/2024 19:17:49
 *  Author: angel
 */ 


#ifndef PMW0_H_
#define PMW0_H_
#include <stdint.h>
#include <avr/io.h>

void init_PMW0A(int orientacion,int modo,int preescaler);
void init_PMW0B(int orientacion);
void duty_cycleA(int duty);
void duty_cycleB(int duty);
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif /* PMW0_H_ */