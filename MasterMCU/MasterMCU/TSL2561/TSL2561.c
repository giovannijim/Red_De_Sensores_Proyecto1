/*
 * TSL2561.c
 *
 * Created: 24/08/2024 12:26:32
 *  Author: Luis Angel Velasquez
 */ 
#include "TSL2561.h"

#define TSL2561 0x39
uint8_t aux, LuzH,LuzL;

void TSL2561_Write(uint8_t reg, uint8_t value){
	uint8_t esclavo = TSL2561 << 1;  // Compone la direcci�n SLA+W

	aux = I2C_INIT();  // Condici�n de inicio
	if (aux != 0x01) {  // Si no se establece la conexi�n
		TWCR |= (1 << TWINT);  // Borrar la bandera
		return;  // No contin�a
	}

	aux = I2C_WRITE(esclavo);  // Direcciona con la SLA+W
	if (aux != 0x01) {  // Si hay alg�n error de comunicaci�n
		I2C_STOP();  // No contin�a, termina la comunicaci�n y la cierra
		return;
	}

	aux = I2C_WRITE(reg);  // Env�a la direcci�n del registro
	if (aux != 0x01) {  // Verifica error al enviar la direcci�n del registro
		I2C_STOP();  // No contin�a, termina la comunicaci�n y la cierra
		return;
	}

	aux = I2C_WRITE(value);  // Env�a el valor a poner en el registro
	if (aux != 0x01) {  // Verifica error al enviar el valor
		I2C_STOP();  // No contin�a, termina la comunicaci�n y la cierra
		return;
	}

	I2C_STOP();  // Finaliza la comunicaci�n y la cierra
}

uint8_t TSL2561_read_reg(uint8_t reg){
	
	//Primera conexi�n como escritura
	uint8_t esclavo = TSL2561 << 1;  // Compone la direcci�n SLA+W
	
	aux = I2C_INIT();  // Condici�n de inicio
	if (aux != 0x01) {  // Si no se establece la conexi�n
		TWCR |= (1 << TWINT);  // Borrar la bandera
		return 1;  // No contin�a
	}
	aux = I2C_WRITE(esclavo);  // Direcciona con la SLA+W
	if (aux != 0x01) {  // Si hay alg�n error de comunicaci�n
		I2C_STOP();  // No contin�a, termina la comunicaci�n y la cierra
		return 1;
	}
	aux = I2C_WRITE(reg);  // Env�a la direcci�n del registro
	if (aux != 0x01) {  // Verifica error al enviar la direcci�n del registro
		I2C_STOP();  // No contin�a, termina la comunicaci�n y la cierra
		return 1;
	}
	
	//Segunda conexi�n como lectura
	esclavo |= 0x01;
	aux = I2C_INIT();  // Condici�n de inicio
	if (aux != 0x01) {  // Si no se establece la conexi�n
		TWCR |= (1 << TWINT);  // Borrar la bandera
		return 1;  // No contin�a
	}
	aux = I2C_WRITE(esclavo);  // Direcciona con la SLA+W
	if (aux != 0x01) {  // Si hay alg�n error de comunicaci�n
		I2C_STOP();  // No contin�a, termina la comunicaci�n y la cierra
		return 1;
	}
	
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete la transmisi�n de la direcci�n
	
	uint8_t dato = TWDR;
	I2C_STOP();  // No contin�a, termina la comunicaci�n y la cierra
	return dato;
	
}

uint16_t TSL2561_read(){
	I2C_SLAVE(0x8C,TSL2561);
	I2C_READ(TSL2561,&LuzL);
	I2C_SLAVE(0x8D,TSL2561);
	I2C_READ(TSL2561,&LuzH);
	
	return ((LuzH << 8) | LuzL);
}

void TSL2561_power_on(void) {
	TSL2561_Write(0x80,0x03);   // Escribir 0x03 en el registro de control (0x80) para encender el sensor
}

void TSL2561_set_timing(uint8_t gain, uint8_t integration_time) {
	uint8_t timing_register = (gain << 4) | integration_time;
	TSL2561_Write(0x81,timing_register); //Escribe timing_register en el registro (0x81) para configurar el sensor
}