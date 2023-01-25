#include "stm32f303xe.h"
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "stdbool.h"

I2C_TypeDef * rtc_i2c ;
uint8_t tm;

static void rtc_write_reg(uint8_t address ,uint8_t reg , uint8_t  data , int len)
{
	uint8_t tmp[2] ;

	tmp[0]  = reg ;
	tmp[1]  = data ;

	I2C_Write(rtc_i2c , address , tmp ,2 ) ;
}


void rtc_init(I2C_TypeDef * dev)
{
	rtc_i2c = dev ;
}



uint8_t rtc_get_reg(uint8_t reg)
{
	uint8_t val ;

	I2C_Write(rtc_i2c , RTC_I2C_ADDR , &reg  , 1) ;

	delay_ms(2);

	I2C_Read(rtc_i2c  , RTC_I2C_ADDR , &val , 1) ;

	return val ;
}

void rtc_get_time(rtc_time_t * rtc_time )
{
	uint8_t tmp = rtc_get_reg(RTC_REG_SECOND) ;
	tm = tmp ;
	//BCD --> DEC
	rtc_time->sec  = ((tmp >> 4) & 0x0f) * 10 + (tmp & 0x0f) ;

	tmp = rtc_get_reg(RTC_REG_MINUTE) ;
	//BCD --> DEC
	rtc_time->min  = ((tmp >> 4) & 0x0f) * 10 + (tmp & 0x0f) ;

	tmp = rtc_get_reg(RTC_REG_HOUR) ;


	if( (tmp & RTC_BIT_MODE_MASK) != 0)
	{
		//12 hour mode
		rtc_time->mode = H_12 ;
		if((tmp & RTC_AM_PM_BIT_MASK) != 0)
		{
			rtc_time->am_pm = PM ;
		}
		else{
			rtc_time->am_pm = AM ;
		}

		rtc_time->hr = ((tmp & 0x1f) >> 4) * 10 + (tmp & 0x0f) ;
	}
	else
	{
		//24 hour mode
		rtc_time->mode = H_24 ;
		tmp = RTC_24H_MASK & tmp ;
		rtc_time->hr = (tmp >> 4) * 10 + (tmp & 0x0f) ;
	}

}

void rtc_set_time(rtc_time_t * rtc_time )
{
	uint8_t high = rtc_time->sec / 10 ;
	uint8_t low  = rtc_time->sec % 10 ;

	uint8_t tmp = (high << 4) | low ;

	rtc_write_reg( RTC_I2C_ADDR ,RTC_REG_SECOND ,tmp , 1) ;

	high = rtc_time->min / 10 ;
	low  = rtc_time->min % 10 ;

	tmp = (high << 4) | low ;

	rtc_write_reg( RTC_I2C_ADDR ,RTC_REG_MINUTE ,tmp , 1) ;


	high = rtc_time->hr / 10 ;
	low  = rtc_time->hr % 10 ;

	tmp = (high << 4) | low ;

	rtc_write_reg( RTC_I2C_ADDR ,RTC_REG_HOUR ,tmp , 1) ;

}


void rtc_set_date(rtc_date_t *date)
{
	uint8_t low  = date->day % 10 ;
	uint8_t high = date->day / 10 ;

	uint8_t tmp  = (high << 4 )|low ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_REG_DATE ,tmp , 1) ;

	low  = date->month % 10 ;
	high = date->month / 10 ;

	tmp  = (high << 4 )|low ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_REG_MONTH ,tmp , 1) ;

	low  = date->year % 10 ;
	high = date->year / 10 ;

	tmp  = (high << 4 )|low ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_REG_YEAR ,tmp , 1) ;

}

void rtc_get_date(rtc_date_t * date)
{
	uint8_t tmp = rtc_get_reg(RTC_REG_DATE) ;

	date->day = (tmp >> 4)*10 + (tmp & 0x0f) ;

	tmp = rtc_get_reg(RTC_REG_MONTH) ;

	tmp &= 0x7f ;

	date->month = (tmp >> 4) *10 +(tmp & 0x0f) ;

	tmp = rtc_get_reg(RTC_REG_YEAR) ;

	date->year = (tmp >> 4) *10 +(tmp & 0x0f) ;
}


void rtc_set_alarm(rtc_time_t * alarm)
{
	uint8_t tmp = GET_BCD_HIGH(alarm->sec)  | GET_BCD_LOW(alarm->sec);

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_SECOND ,tmp , 1) ;

	tmp = GET_BCD_HIGH(alarm->min)  | GET_BCD_LOW(alarm->min)   ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_MINUTE ,tmp , 1) ;

	tmp = GET_BCD_HIGH(alarm->hr)  | GET_BCD_LOW(alarm->hr)     ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_HOUR ,tmp , 1) ;

	//set alarm when sec ,mi, and hour matchs

	tmp = rtc_get_reg(RTC_A1_DATE) ;

	//set A1M4 bit to one
	tmp |= 0x80 ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_DATE ,tmp , 1) ;

	//clear alarm flag
	tmp  = rtc_get_reg(RTC_STATUS_REG) & 0xfe; // set A1F to 0

	rtc_write_reg( RTC_I2C_ADDR , RTC_STATUS_REG ,tmp , 1) ;

	//enable rtc alarm
	tmp = rtc_get_reg(RTC_CONTROL_REG) ;

	//set AE1 in control reg
	tmp |= 0x01;

	rtc_write_reg( RTC_I2C_ADDR , RTC_CONTROL_REG ,tmp , 1) ;

}

void rtc_get_alarm(rtc_time_t * tm)
{
	uint8_t tmp = rtc_get_reg(RTC_A1_SECOND) ;

	tm->sec = (tmp >> 4) *10 + (tmp & 0x0f) ;

	tmp = rtc_get_reg(RTC_A1_MINUTE) ;

	tm->min = (tmp >> 4) *10 + (tmp & 0x0f) ;

	tmp = rtc_get_reg(RTC_A1_HOUR) ;

	tm->hr = (tmp >> 4) *10 + (tmp & 0x0f) ;
}


bool rtc_is_alarm_set( )
{
	bool ret = false ;
	//check is A1F in status register is set
	if( (rtc_get_reg(RTC_STATUS_REG) & RTC_STATUS_AF1_MASK ) != 0 )
	{
		ret = true ;
	}
	else{
		ret = false ;
	}

	return ret ;
}

void rtc_alarm_clear()
{
	//clear A1F in status register
	uint8_t tmp = rtc_get_reg(RTC_STATUS_REG) ;

	tmp &= ~RTC_STATUS_AF1_MASK ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_STATUS_REG ,tmp , 1) ;
}

