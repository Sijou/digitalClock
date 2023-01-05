/*
 * rtc.h
 *
 *  Created on: 04.01.2023
 *      Author: Lenovo
 */

#ifndef RTC_RTC_H_
#define RTC_RTC_H_

#include "stm32f303xe.h"

//all available register on RTC
#define RTC_I2C_ADDR 	0x68

#define RTC_REG_SECOND 	0x00
#define RTC_REG_MINUTE 	0x01
#define RTC_REG_HOUR  	0x02
#define RTC_REG_DOW    	0x03

#define RTC_REG_DATE   	0x04
#define RTC_REG_MONTH  	0x05
//#define RTC_CENTURY 		7
#define RTC_REG_YEAR   	0x06

#define RTC_A1_SECOND	0x07
#define RTC_A1_MINUTE	0x08
#define RTC_A1_HOUR		0x09
#define RTC_A1_DATE		0x0a

#define RTC_A2_MINUTE	0x0b
#define RTC_A2_HOUR		0x0c
#define RTC_A2_DATE		0x0d


typedef struct {
	uint8_t sec ;
	uint8_t min ;
	uint8_t hr  ;
}rtc_time_t;

void rtc_get_time(rtc_time_t * rtc_time ) ;
void rtc_init(I2C_TypeDef * dev) ;

void rtc_I2C_Calculate(uint8_t *bufferReceive, uint8_t *bufferCalculated);
void rtc_I2C_TimeOffset(void);
void rtc_I2c_ReadAll(uint8_t *rtc_bufferReceive);
void rtc_I2C_Calculate(uint8_t *rtc_bufferReceive, uint8_t *rtc_bufferOut);

#endif /* RTC_RTC_H_ */
