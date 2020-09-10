/*
* Driver I2C para Proyecto - Pulsera Saturometrica.
* Util para implementar rapidamente comunicacion simple I2C.
* Basado en el driver I2C desarrollado por RedBearLab

Copyright (c) 2012-2014 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "I2C_Driver.h"
#include "mbed.h"
#include "wire.h"


TwoWire Wire = TwoWire(NRF_TWI0);

void ini_i2c( int scl, int sda){
    Wire.begin(scl, sda, TWI_FREQUENCY_100K);
}

/*
WriteByte( addr, *pbuf, length, dev_addr) : escritura i2c en la dirección i2c definida al inicio del programa.

addr:  dirección del registro a ser escrito.
*pbuf:    dirección (puntero) del array de datos a ser escritos. Dirección 0: LSB
length:   cantidad de bytes a ser transmitidos.
dev_addr: Direección física del dispositivo esclavo i2c

*/

int write_i2c(uint8_t addr, uint8_t *pbuf, uint16_t length, uint8_t dev_addr){ //Parámetros: addr: dirección de memoria; *pbuf: puntero de byte 0 del buffer; length: cantidad de bytes a enviar; dev_addr: Direección física del dispositivo esclavo i2c
    int aux=0;
    uint8_t aux2[length];
    uint8_t *aux3;
    for ( aux = 0; aux < length; aux++ ){
        aux3 = pbuf+length-(aux+1);
        aux2[aux]= *aux3;
    }

    Wire.beginTransmission(dev_addr);
    Wire.write( (uint8_t)addr );
    Wire.write(aux2, length);
    return (Wire.endTransmission());
}

/*
ReadByte( addr, *pbuf, length, dev_addr) : lectura i2c en la dirección i2c definida al inicio del programa.

addr:  dirección del registro a ser escrito.
*pbuf:    dirección (puntero) del array de datos a ser escritos. Dirección 0: LSB
length:   cantidad de bytes a ser transmitidos.
dev_addr: Direección física del dispositivo esclavo i2c

*/

int read_i2c(uint8_t addr, uint8_t *pbuf, uint16_t length, uint8_t dev_addr){ //Parámetros: addr: dirección de memoria; *pbuf: puntero de byte 0 del buffer; length: cantidad de bytes a enviar; dev_addr: Direección física del dispositivo esclavo i2c
    int error=0;
    
    Wire.beginTransmission(dev_addr);
    Wire.write( (uint8_t)addr );    
    error = Wire.endTransmission();
    
    Wire.requestFrom(dev_addr+1, length);
    
    pbuf=pbuf+length-1;
    while( Wire.available() > 0 )
    {
        *pbuf = Wire.read();
        pbuf--;
    }
    return(error);
}