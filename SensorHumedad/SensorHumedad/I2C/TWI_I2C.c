/*
 * TWI_I2C.c
 * Created: 8/2/2024 8:10:41 PM
 *  Author: alesa
 */ 

#include "TWI_I2C.h"

uint8_t esclavo, dato, aux;

#define SLA_R(address) ((address << 1) | 1)
	
void I2C_SETTING_MASTER(uint8_t Prescaler, unsigned long SCL_Clock){
	DDRC &= ~((1<<DDC4) | (1<<DDC5));
	
	TWBR = ((F_CPU/SCL_Clock)-16)/(2*Prescaler);
	
	
	switch(Prescaler){
		case 1:
			TWSR &= ~((1<<TWPS1) | (1<<TWPS0));
			break;
		case 4:
			TWSR &= ~(1<<TWPS1);
			TWSR |= (1<<TWPS0);
			break;
		case 16:
			TWSR &= ~(1<<TWPS0);
			TWSR |= (1<<TWPS1);
			break;
			
		case 64:
			TWSR |= ~(1<<TWPS1) | (1<<TWPS0);
			break;
		default:
			TWSR &= ~((1<<TWPS1) | (1<<TWPS0));
			Prescaler = 1;
			break;
			
	}
	
	TWCR = 1 << TWEN;   //Habilita la interfaz
}

void I2C_SETTING_SLAVE(uint8_t address){
	DDRC &= ~((1<<DDC4) | (1<<DDC5));
	
	address <<= 1;  //Ubica la direcci�n y
	address |= 0x01; //habilita para reconocer las llamadas generales de I2C
	TWAR = address;
	
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE); //Habilita el BUS, con reconocimiento e interrupci�n
}


uint8_t I2C_INIT(){
	uint8_t estado;  //Variable que indica que no se consigui� el bus 
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA); //Condici�n de inicio
	while (!(TWCR & (1 << TWINT)));  //Espera la bandera TWINT
	estado = TWSR & 0xF8; //Obtiene el estado, 1 condicion de inicio, o 2 inicio repetido
	
	if (estado == 0x08 || estado == 0x10) 
	{
		return 1;
	}
	
	return estado;  
	
}


//I2C_EscByte: Escribe un dato o una SLA + W/R por el bus
//Recibe: El dato de 8 bits a enviar
//Regresa: 0x01 envio sin problemas, edo, si el dato no se envio como se esperaba

uint8_t I2C_WRITE(uint8_t dato){
	uint8_t estado;
	
	TWDR = dato;      //Carga el dato   
	TWCR = (1 << TWEN) | (1 << TWINT);   //Inicia el envio
	
	while (!(TWCR & (1 << TWINT))); //espera la bandera TWINT
	estado = TWSR & 0xF8; //Obtiene el estado
	
	    //Hay 3 posibilidades de �xito:
	if (estado == 0x18 || estado == 0x28 || estado == 0x40)  //Transmiti� una SLA+W CON ACK, transimiti� una SLA+R con ACK, Transmiti� un dato con ACK
	{
		return 0x01;
	}
	return estado;   //So hay alg�n error
	
}

void I2C_STOP(){
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  //Condici�n de Paro
	while (TWCR & (1 << TWSTO));  //El bit se limpia por HW
}

void I2C_SLAVE(uint8_t dato, uint8_t address){
	esclavo = address;  //Direcci�n del esclavo
	esclavo = esclavo << 1;    //Compone la SLA+W*************************************************************************
	
	
	aux = I2C_INIT();   //Condicion de inicio
	if (aux != 0x01)  //Si no se establece la conexi�n
	{
		TWCR |= (1 << TWINT);  //Borrar la bandera
		return;   //No continua
	}
	
	aux = I2C_WRITE(esclavo);     //Direcciona con la SLA+W
	if (aux != 0x01)   //Si hay algun error de comunicaci�n
	{
		I2C_STOP();  //No continua, termina la comunicaci�n y la cierra
		return;   
	}
	
	I2C_WRITE(dato);   //Envia el dato al esclavo
	I2C_STOP();  //No continua, termina la comunicaci�n y la cierra
	
	
}

// Lee un byte de datos del esclavo I2C
uint8_t I2C_READ(uint8_t address, uint8_t *dato) {
	// Iniciar condici�n de START
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete

	// Verificar condici�n de START
	if ((TWSR & 0xF8) != TW_START) {
		return 1; // Error: No se pudo iniciar la condici�n de START
	}

	// Enviar direcci�n del esclavo con bit de lectura
	TWDR = SLA_R(address);
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete la transmisi�n de la direcci�n

	// Verificar ACK/NACK de la direcci�n
	uint8_t twsr_value = TWSR & 0xF8;
	if (twsr_value == TW_MR_SLA_NACK) {
		I2C_STOP(); // Detener si se recibe un NACK
		return 1; // Error: NACK recibido
		} else if (twsr_value != TW_MR_SLA_ACK) {
		I2C_STOP(); // Detener si otro estado inesperado
		return 1; // Error: Estado inesperado
	}

	// Leer un byte del esclavo
	TWCR = (1 << TWEN) | (1 << TWINT); // Leer sin ACK (NACK)
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete la recepci�n

	// Verificar estado de recepci�n
	twsr_value = TWSR & 0xF8;
	if (twsr_value != TW_MR_DATA_NACK && twsr_value != TW_MR_DATA_ACK) {
		I2C_STOP(); // Detener si hay error en la recepci�n
		return 1; // Error en la recepci�n
	}

	// Leer el dato recibido
	*dato = TWDR;

	// Generar condici�n de STOP
      I2C_STOP();
  
	return  *dato; // �xito
}