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
#include "I2C/TWI_I2C.h"
#include "LCD/LCD_8BITS.h"
#include "UART/UART.h"

//Se definen las siguientes variables
#define sensorHumedad 0x30
#define sensorTemperatura 0x40
#define sensorI2C 0x50

//Subrutinas:
void float_to_char_s1(uint8_t value_adch);

//Se crean e identifican las distitnas variables y listas:
uint8_t BufferRX;
uint8_t BufferSensorHumedad = 0;
uint8_t BufferSensorTemperatura = 0;
uint8_t data = 0;
float voltage;
char buffer_s1[5] = {'0'};
char buffer_s2[6] = {'0'};
uint8_t receivedChar;


int main(void)
{
	cli();						// Se apagan las interrupciones globales
	DDRB |= 1<<DDB4;
	PORTB &= ~1<<PORTB4;
	init_8bits_LCD();			// Se inicializa la pantalla LCD
	initUART9600();				// Se inicializa la comunicacion UART
	I2C_SETTING_MASTER(4, 200);	// Se inicializa el microcontrolador como Master I2C
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
		
		//I2C_SLAVE(1,sensorTemperatura);
		//_delay_ms(1000);
    }
}


// Vector de interrupcion UART
ISR(USART_RX_vect)
{
	BufferRX = UDR0;
	// En funcion de lo que se haya recibido se ejecutara una accion
	if (UDR0 == 'b'){
		writeUART(BufferSensorHumedad);}
	if (UDR0 == 'a'){
		writeUART(BufferSensorTemperatura);}
	if (UDR0 == '1'){
		I2C_SLAVE(1,sensorTemperatura);}
	if (UDR0 == '2'){
		I2C_SLAVE(2,sensorTemperatura);}
}


// Subrutina para convertir una variable de tipo float (con valores decimales) a una cadena Char
void float_to_char_s1(uint8_t value_adch){
	int a, b;									// Se designa el tipo de variable de a y b
	voltage = (float)value_adch * 100.0f/255.0f;		// Se realizar una regla de 3 para escalar [0-255] a [0.00-100.00]
	
	a = (int)voltage;							// Se obtienen las cifras enteras de la variable float
	b = ((int)(voltage*100.0f))%100;				// Se obtienen las cifras decimales de la variable float
	
	snprintf(buffer_s1, 5, "%d.%02d", a, b);		// Se convierte une todo en una cadena char
}