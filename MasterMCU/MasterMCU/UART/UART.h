/*
 * UART.h
 * Created: 7/19/2024 12:55:17 PM
 *  Author: Giovanni
 */ 

#ifndef UART_H_
#define UART_H_

// Inicializar el protocolo UART

void initUART9600(void);

// Escribir UART
void writeUART(char Caracter);

//Enviar una cadena
void cadena (char* texto);

//Menu de trabajo
void Menu (char* text);

//Menu de trabajo
void Respuesta (uint8_t response);

//Recibir UART
char uart_receive(void);

#endif /* UART_H_ */