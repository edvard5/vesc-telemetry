

#ifndef MBED_I2C_DRIVER_H
#define MBED_I2C_DRIVER_H

#include "mbed.h"
#include "wire.h"

void ini_i2c( int scl, int sda);
int write_i2c(uint8_t addr, uint8_t *pbuf, uint16_t length, uint8_t dev_addr); //Definicion de rutina de escritura.
int read_i2c(uint8_t addr, uint8_t *pbuf, uint16_t length, uint8_t dev_addr); //Definicion de rutina de lectura.

#endif

