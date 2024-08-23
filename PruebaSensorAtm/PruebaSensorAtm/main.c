#define F_CPU 16000000
#define BME280_ADDRESS 0x76

// Registros del BME280
#define BME280_REG_ID 0xD0
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG 0xF5
#define BME280_REG_PRESS_MSB 0xF7

#include <avr/io.h>
#include <util/delay.h>
#include "I2C/TWI_I2C.h"
#include "UART/UART.h"
#include <stdio.h>

// Variables de compensación de presión
uint16_t dig_P1;
int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

// Función para inicializar el BME280
void bme280_init(void) {
	// Configurar humedad
	I2C_SLAVE(BME280_REG_CTRL_HUM, BME280_ADDRESS);
	I2C_SLAVE(0x01, BME280_ADDRESS);

	// Configurar temperatura y presión
	I2C_SLAVE(BME280_REG_CTRL_MEAS, BME280_ADDRESS);
	I2C_SLAVE(0x27, BME280_ADDRESS);

	// Configurar filtro y tiempo de espera
	I2C_SLAVE(BME280_REG_CONFIG, BME280_ADDRESS);
	I2C_SLAVE(0xA0, BME280_ADDRESS);
}

// Función para leer los registros de calibración de presión
void bme280_read_calibration(void) {
	uint8_t data[18];

	// Leer registros de calibración de presión
	for (uint8_t i = 0; i < 18; i++) {
		I2C_READ(BME280_ADDRESS, &data[i]);
	}

	dig_P1 = (data[1] << 8) | data[0];
	dig_P2 = (data[3] << 8) | data[2];
	dig_P3 = (data[5] << 8) | data[4];
	dig_P4 = (data[7] << 8) | data[6];
	dig_P5 = (data[9] << 8) | data[8];
	dig_P6 = (data[11] << 8) | data[10];
	dig_P7 = (data[13] << 8) | data[12];
	dig_P8 = (data[15] << 8) | data[14];
	dig_P9 = (data[17] << 8) | data[16];
}

// Función para leer la presión
uint32_t bme280_read_pressure(void) {
	int32_t adc_P, var1, var2;
	uint32_t P;
	uint8_t data[3];

	// Leer datos de presión
	for (uint8_t i = 0; i < 3; i++) {
		I2C_READ(BME280_ADDRESS, &data[i]);
	}

	adc_P = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[2] >> 4);

	// Calcular la presión en Pa (Pascal)
	var1 = (((int32_t)adc_P >> 3) - ((int32_t)dig_P1 << 1)) * ((int32_t)dig_P2) >> 11;
	var2 = (((((int32_t)adc_P >> 4) - ((int32_t)dig_P1)) * ((adc_P >> 4) - ((int32_t)dig_P1))) >> 12) * ((int32_t)dig_P3) >> 14;
	P = ((var1 + var2) >> 10) + ((uint32_t)dig_P4 << 4);

	return P;
}

// Función para enviar la presión por UART
void uart_send_pressure(uint32_t pressure) {
	char buffer[30];
	int length = sprintf(buffer, "Pressure: %lu Pa\n", pressure);
	cadena(buffer); // Enviar cadena a través de UART
}

int main(void) {
	// Inicializar UART y I2C
	initUART9600();
	I2C_SETTING_MASTER(1, 100000); // Configurar I2C a 100kHz
	bme280_init();
	bme280_read_calibration();

	while (1) {
		writeUART('1');
		uint32_t pressure = bme280_read_pressure();
		uart_send_pressure(pressure); // Enviar presión a través de UART
		_delay_ms(1000); // Esperar 1 segundo antes de la siguiente lectura
	}
}