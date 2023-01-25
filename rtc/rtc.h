/*
 * rtc.h
 *
 *  Created on: 4 janv. 2023
 *      Author: medali
 */

#ifndef RTC_RTC_H_
#define RTC_RTC_H_

/*****
 * Datasheet link : https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf
 *
 *
 *
 */
#include "stm32f303xe.h"
#include "stdbool.h"

//all available register on RTC


#define RTC_I2C_ADDR 	   0x68

#define RTC_REG_SECOND 	   0x00
#define RTC_REG_MINUTE 	   0x01
#define RTC_REG_HOUR  	   0x02
#define RTC_REG_DOW        0x03

#define RTC_REG_DATE   	   0x04
#define RTC_REG_MONTH  	   0x05
//#define RTC_CENTURY 		7
#define RTC_REG_YEAR       0x06

#define RTC_A1_SECOND	   0x07
#define RTC_A1_MINUTE	   0x08
#define RTC_A1_HOUR		   0x09
#define RTC_A1_DATE		   0x0a

#define RTC_A2_MINUTE	   0x0b
#define RTC_A2_HOUR		   0x0c
#define RTC_A2_DATE		   0x0d
#define RTC_CONTROL_REG    0x0E
#define RTC_STATUS_REG     0x0F

#define RTC_BIT_MODE_MASK  0x40
#define RTC_24H_MASK       0x3f
#define RTC_AM_PM_BIT_MASK 0x20

#define RTC_STATUS_AF1_MASK  0x01

#define  GET_BCD_HIGH(X)  ((X / 10)<<4)
#define  GET_BCD_LOW(X)   (X % 10)

typedef enum {
	H_24 ,
	H_12 ,
}clock_mode_t;
typedef enum{
	AM ,
	PM
}AM_PM_t;

typedef struct {
	uint8_t sec 	  ;
	uint8_t min 	  ;
	uint8_t hr  	  ;
	clock_mode_t mode ;
	AM_PM_t am_pm     ;//used only in H_12 mode

}rtc_time_t;

typedef struct {
	uint8_t day ;
	uint8_t month ;
	uint8_t year ; // 0-99
}rtc_date_t;



void rtc_init(I2C_TypeDef * dev) ;

void rtc_get_time(rtc_time_t * rtc_time ) ;

void rtc_set_time(rtc_time_t * rtc_time ) ;

void rtc_set_date(rtc_date_t *date) ;

void rtc_get_date(rtc_date_t * date) ;

void rtc_set_alarm(rtc_time_t * alarm) ;

//test purposes
void rtc_get_alarm(rtc_time_t * tm)    ;

bool rtc_is_alarm_set( ) ;

void rtc_alarm_clear() ;

uint8_t rtc_get_reg(uint8_t reg) ;


#endif /* RTC_RTC_H_ */
