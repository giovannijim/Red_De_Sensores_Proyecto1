/*
 * MasterMCU.c
 * Created: 8/13/2024 10:57:06 AM
 * Author 1: Giovanni Jimenez
 * Author 2: Luisa Angela
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#include "I2C/TWI_I2C.h"
#include "LCD/LCD_8BITS.h"
#include "UART/UART.h"

#define sensorHumedad 0x30
#define sensorTemperatura 0x40
#define sensorI2C 0x50

void float_to_char_s1(uint8_t value_adch);

uint8_t BufferRX;
uint8_t BufferSensorHumedad;
uint8_t BufferSensorTemperatura;
uint8_t data = 0;
float voltage;
char buffer_s1[5] = {'0'};
char buffer_s2[6] = {'0'};
uint8_t receivedChar;

int main(void)
{
	cli();
	init_8bits_LCD();
	initUART9600();
	I2C_SETTING_MASTER(4, 200);
	sei();
	
	LCD_Set_Cursor(0,1);		// Se establece el cursor en y, x
	// Se escriben los siguientes caracteres en la pantalla
	LCD_Write_Char('H');
	LCD_Write_Char('U');
	LCD_Write_Char('M');
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
	
	
    while (1) 
    {
		/*if (UCSR0A & (1 << RXC0)) {
			receivedChar = UDR0;
			// Procesa el dato recibido
		}*/
		//char receivedChar = uart_receive();
		
		BufferSensorHumedad = I2C_READ(sensorHumedad, &data);
		float_to_char_s1(BufferSensorHumedad);
		LCD_Set_Cursor(1,1);		// Se establece el cursor en y, x
		LCD_Write_String(buffer_s1);
		
	/*	if (receivedChar == 'a')
		{
			// Enviar de vuelta el dato para verificar la recepción
			while (!(UCSR0A & (1 << UDRE0)));  // Esperar a que el buffer de transmisión esté vacío
			UDR0 = BufferSensorHumedad;
		}*/
		
		BufferSensorTemperatura = 255;
		snprintf(buffer_s2, sizeof(buffer_s2), "%d.%d C", BufferSensorTemperatura / 8, (BufferSensorTemperatura % 8) * 100 / 8);
		LCD_Set_Cursor(1,6);		// Se establece el cursor en y, x
		LCD_Write_String(buffer_s2);
    }
}


// Vector de interrupcion UART
ISR(USART_RX_vect)
{
	BufferRX = UDR0;
	if (UDR0 == 'a')
	{
		PORTB |= 1<<PORTB5;
		writeUART(BufferSensorHumedad);
	}
	else{
		PORTB &= ~1<<PORTB5;
	}
}


// Subrutina para convertir una variable de tipo float (con valores decimales) a una cadena Char
void float_to_char_s1(uint8_t value_adch){
	int a, b;									// Se designa el tipo de variable de a y b
	voltage = value_adch * 0.01960784314;		// Se realizar una regla de 3 para escalar [0-255] a [0.00-5.00]
	
	a = (int)voltage;							// Se obtienen las cifras enteras de la variable float
	b = ((int)(voltage*1000))%1000;				// Se obtienen las cifras decimales de la variable float
	
	snprintf(buffer_s1, 5, "%d.%2.2d", a, b);		// Se convierte une todo en una cadena char
}