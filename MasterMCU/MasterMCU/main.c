/*
 * MasterMCU.c
 * Created: 8/13/2024 10:57:06 AM
 * Author 1: Giovanni Jimenez
 * Author 2: Luisa Angela
 */ 

//Se incluyen librerias Default
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Se incluyen librerias creadas por nosostros:
#include "TSL2561/I2C/TWI_I2C.h"
#include "LCD/LCD_8BITS.h"
#include "UART/UART.h"
#include "TSL2561/TSL2561.h"

//Se definen las siguientes variables
#define sensorHumedad 0x30
#define sensorTemperatura 0x40
#define sensorI2C 0x50

//Subrutinas:
void float_to_char_s1(uint8_t value_adch);
void floatToChar6(float num);
uint16_t REGISTROS();
void enviarDecimales(float num);
long map(long x, long in_min, long in_max, long out_min, long out_max);

//Se crean e identifican las distitnas variables y listas:
uint8_t BufferRX;
uint8_t BufferSensorHumedad = 0;
uint8_t BufferSensorTemperatura = 0;
uint16_t BufferSensorLuz = 0;
uint8_t BufferSensorLuz_8t=0;
long BufferSensorLuz_l;
uint8_t data = 0;
float voltage;
float porcentaje;
char buffer_s1[5] = {'0'};
char buffer_s2[6] = {'0'};
char buffer_s3[6] = {"13.2"};
char buffaux[6] = {'0'};
uint8_t receivedChar;
uint8_t LuzH,LuzL;


int main(void)
{
	cli();						// Se apagan las interrupciones globales
	DDRB |= 1<<DDB4;
	PORTB &= ~1<<PORTB4;
	init_8bits_LCD();			// Se inicializa la pantalla LCD
	initUART9600();				// Se inicializa la comunicacion UART
	I2C_SETTING_MASTER(4, 200);	// Se inicializa el microcontrolador como Master I2C
	TSL2561_power_on();		   //Enciende el sensor
	TSL2561_set_timing(1, 2);  // Configurar ganancia alta y tiempo de integración de 402 ms
	
	sei();						// Se activan las interrupciones globales
	
	LCD_Set_Cursor(0,1);		// Se establece el cursor en y, x
	// Se escriben los siguientes caracteres en la pantalla
	LCD_Write_Char('H');
	LCD_Write_Char('U');
	LCD_Write_Char('M');
	LCD_Write_Char('%');
	
	LCD_Set_Cursor(0,7);		// Se establece el cursor en y, x
	// Se escriben los siguientes caracteres en la pantalla
	LCD_Write_Char('T');
	LCD_Write_Char('M');
	LCD_Write_Char('P');
	
	LCD_Set_Cursor(0,12);		// Se establece el cursor en y, x
	// Se escriben los siguientes caracteres en la pantalla
	LCD_Write_Char('I');
	LCD_Write_Char('2');
	LCD_Write_Char('C');
	LCD_Write_Char('%');
	/*
	LCD_Set_Cursor(1,5);		// Se establece el cursor en y, x
	// Se escriben los siguientes caracteres en la pantalla
	LCD_Write_Char('%');
	*/
	
	
    while (1) 
    {
		BufferSensorHumedad = I2C_READ(sensorHumedad, &data);
		float_to_char_s1(BufferSensorHumedad);
		LCD_Set_Cursor(1,1);		// Se establece el cursor en y, x
		LCD_Write_String(buffer_s1);
		
		_delay_ms(10);
		
		BufferSensorTemperatura = I2C_READ(sensorTemperatura, &data);
		snprintf(buffer_s2, sizeof(buffer_s2), "%dC", BufferSensorTemperatura);
		LCD_Set_Cursor(1,7);		// Se establece el cursor en y, x
		LCD_Write_String(buffer_s2);
		
		BufferSensorLuz = REGISTROS();
		BufferSensorLuz_l=map(BufferSensorLuz,0,65535,0,255);
		BufferSensorLuz_8t=(uint8_t)BufferSensorLuz_l;
		
		porcentaje = (BufferSensorLuz_8t / 255.0) * 100;
		
		floatToChar6(porcentaje);
		buffer_s3[0] = buffaux[0];
		buffer_s3[1] = buffaux[1];
		buffer_s3[2] = buffaux[2];
		buffer_s3[3] = buffaux[3];
		LCD_Set_Cursor(1,11);		// Se establece el cursor en y, x
		LCD_Write_String(buffer_s3);
		
		//I2C_SLAVE(1,sensorTemperatura);
		//_delay_ms(1000);
    }
}


// Vector de interrupcion UART
ISR(USART_RX_vect)
{
	BufferRX = UDR0;
	_delay_ms(500);
	// En funcion de lo que se haya recibido se ejecutara una accion
	if (UDR0 == 's'){
		writeUART(BufferSensorHumedad);
		writeUART(BufferSensorTemperatura);
		writeUART(BufferSensorLuz_8t);}
		
//enviarDecimales(porcentaje);}
			
	if (UDR0 == '1'){
		I2C_SLAVE(1,sensorTemperatura);}
	if (UDR0 == '2'){
		I2C_SLAVE(2,sensorTemperatura);}
	if (UDR0 == '3'){
		I2C_SLAVE(3,sensorHumedad);}
}


// Subrutina para convertir una variable de tipo float (con valores decimales) a una cadena Char
void float_to_char_s1(uint8_t value_adch){
	int a, b;									// Se designa el tipo de variable de a y b
	voltage = (float)value_adch * 100.0f/255.0f;		// Se realizar una regla de 3 para escalar [0-255] a [0.00-100.00]
	
	a = (int)voltage;							// Se obtienen las cifras enteras de la variable float
	b = ((int)(voltage*100.0f))%100;				// Se obtienen las cifras decimales de la variable float
	
	snprintf(buffer_s1, 5, "%d.%02d", a, b);		// Se convierte une todo en una cadena char
}

void floatToChar6(float num) {
	// Variables para separar la parte entera y la parte decimal
	int parteEntera = (int)num;
	int parteDecimal = (int)((num - parteEntera) * 10); // 1 decimal

	// Control para evitar números negativos en la parte decimal
	if (parteDecimal < 0) {
		parteDecimal = -parteDecimal;
	}

	// Llenar el buffer dependiendo de la cantidad de dígitos de la parte entera
	if (parteEntera >= 100) {
		buffaux[0] = (parteEntera / 100) + '0';            // Centenas
		buffaux[1] = ((parteEntera / 10) % 10) + '0';      // Decenas
		buffaux[2] = (parteEntera % 10) + '0';             // Unidades
		buffaux[3] = '.';                                  // Punto decimal
		buffaux[4] = parteDecimal + '0';                   // Primer decimal
		buffaux[5] = '\0';                                 // Fin de la cadena
		} else if (parteEntera >= 10) {
		buffaux[0] = (parteEntera / 10) + '0';             // Decenas
		buffaux[1] = (parteEntera % 10) + '0';             // Unidades
		buffaux[2] = '.';                                  // Punto decimal
		buffaux[3] = parteDecimal + '0';                   // Primer decimal
		buffaux[4] = '\0';                                 // Fin de la cadena
		buffaux[5] = '\0';                                 // Relleno extra si fuera necesario
		} else {
		buffaux[0] = parteEntera + '0';                    // Unidades
		buffaux[1] = '.';                                  // Punto decimal
		buffaux[2] = parteDecimal + '0';                   // Primer decimal
		buffaux[3] = '\0';                                 // Fin de la cadena
		buffaux[4] = '\0';                                 // Relleno extra si fuera necesario
		buffaux[5] = '\0';                                 // Relleno extra si fuera necesario

	}
}

uint16_t REGISTROS(){
	LuzL=TSL2561_read_reg(0x8C);
	LuzH=TSL2561_read_reg(0x8D);
	return ((LuzH << 8) | LuzL);
	
}

void enviarDecimales(float num){
	uint8_t entero = (int) num;
	int decimal = (int)((num - entero)*10);
	uint8_t decimal_8t = (uint8_t) decimal;
	uint8_t entero_8t = (uint8_t) entero;
	writeUART(entero_8t);
	writeUART(decimal_8t);
}

// Función para mapear un valor de uint16_t a uint8_t
long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}