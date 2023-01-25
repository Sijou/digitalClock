/*
 * i2c.h
 *
 *  Created on: 22 déc. 2022
 *      Author: medali
 */

#ifndef DRIVER_INC_I2C_H_
#define DRIVER_INC_I2C_H_
#include "stm32f303xe.h"

#define I2C_TIMOUT			100 //ms


/*
 * I2C Error code
 */

#define ERROR_OK   			     (0)
#define ERROR_ADDRESS_NOT_FOUND  (-1)
#define ERROR_TIMEOUT      		 (-2)
#define ERROR_NANK_RECEIVED      (-3)
#define ERRROR_TIMEOUT1


void I2C_Init(I2C_TypeDef * i2c) ;

int8_t search_address(I2C_TypeDef * i2c , uint8_t * address) ;

void I2C_Write(I2C_TypeDef * i2c , uint8_t address , uint8_t * data , uint8_t len);

void I2C_Read(I2C_TypeDef * i2c,uint8_t address , uint8_t * buffer , int len) ;

#endif /* DRIVER_INC_I2C_H_ */
