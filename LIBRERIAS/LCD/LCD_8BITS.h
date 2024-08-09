/*
 * LCD_8BITS.h
 * Created: 7/18/2024 11:09:43 AM
 *  Author: Giovanni
 */ 


#ifndef LCD_8BITS_H_
#define LCD_8BITS_H_

// Funcion para colocar en el puerto un valor
void LCD_Port(char a);

// Funcion para mandar un comando
void LCD_CMD(char a);

// Inicializar con 8 bits el LCD
void init_8bits_LCD (void);

// Funcion para enviar un caracter
void LCD_Write_Char(char c);

// Funcion para mandar una cadena de caracteres
void LCD_Write_String(char *a);

//Desplazamiento hacia la derecha
void LCD_Shift_Right(void);

//Desplazamiento hacia la izquierda
void LCD_Shift_Left(void);

// Establecer cursor
void LCD_Set_Cursor(char c, char f);

#endif /* LCD_8BITS_H_ */