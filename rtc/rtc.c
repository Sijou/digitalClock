#include "stm32f303xe.h"
#include "main.h"
#include "i2c.h"
#include "rtc.h"

I2C_TypeDef * rtc_i2c ;
uint8_t tm;

static void rtc_write_reg(uint8_t address ,uint8_t reg , uint8_t  data , int len)
{
	uint8_t tmp[2] ;

	tmp[0]  = reg ;
	tmp[1]  = data ;

	I2C_Write(rtc_i2c , address , tmp ,2 ) ;
}

static void rtc_read_reg(uint8_t address , uint8_t reg , uint8_t * data , int len)
{
	I2C_Write(rtc_i2c , address , &reg , 1 ) ;
	I2C_Read(rtc_i2c , address , &data[reg] , 1) ;
}

void rtc_init(I2C_TypeDef * dev)
{
	rtc_i2c = dev ;
}

void rtc_I2C_TimeOffset(void)
{
	rtc_write_reg( RTC_I2C_ADDR , RTC_REG_HOUR ,0x00 , 1) ;

	//******Offset Real Time *****************
	rtc_write_reg(RTC_I2C_ADDR, RTC_REG_SECOND, 0x52, 1);	//second
	rtc_write_reg(RTC_I2C_ADDR, RTC_REG_MINUTE, 0x55, 1);	//minute
	rtc_write_reg(RTC_I2C_ADDR, RTC_REG_HOUR, 0x09, 1);		//hour
	rtc_write_reg(RTC_I2C_ADDR, RTC_REG_DATE, 0x17, 1);		//day
	rtc_write_reg(RTC_I2C_ADDR, RTC_REG_MONTH, 0x12, 1);	//month
	rtc_write_reg(RTC_I2C_ADDR, RTC_REG_YEAR, 0x22, 1);		//year (changeable from 2000-2099)
}


void rtc_I2c_ReadAll(uint8_t *rtc_bufferReceive)
{
	rtc_read_reg(RTC_I2C_ADDR, RTC_REG_SECOND, rtc_bufferReceive, sizeof(rtc_bufferReceive));
	rtc_read_reg(RTC_I2C_ADDR, RTC_REG_MINUTE, rtc_bufferReceive, sizeof(rtc_bufferReceive));
	rtc_read_reg(RTC_I2C_ADDR, RTC_REG_HOUR, rtc_bufferReceive, sizeof(rtc_bufferReceive));
	rtc_read_reg(RTC_I2C_ADDR, RTC_REG_DOW, rtc_bufferReceive, sizeof(rtc_bufferReceive));
	rtc_read_reg(RTC_I2C_ADDR, RTC_REG_DATE, rtc_bufferReceive, sizeof(rtc_bufferReceive));
	rtc_read_reg(RTC_I2C_ADDR, RTC_REG_MONTH, rtc_bufferReceive, sizeof(rtc_bufferReceive));
	rtc_read_reg(RTC_I2C_ADDR, RTC_REG_YEAR, rtc_bufferReceive, sizeof(rtc_bufferReceive));
}


void rtc_I2C_Calculate(uint8_t *rtc_bufferReceive, uint8_t *rtc_bufferOut)
{
	//rtc_bufferOut[13];

	uint8_t sec_einer = 0;
	uint8_t sec_zehner = 0;
	uint8_t min_einer = 0;
	uint8_t min_zehner = 0;
	uint8_t hour_einer = 0;
	uint8_t hour_zehner = 0;
	uint8_t day_einer = 0;
	uint8_t date_einer = 0;
	uint8_t date_zehner = 0;
	uint8_t month_einer = 0;
	uint8_t month_zehner = 0;
	uint8_t year_einer = 0; // ggf mit uint16_t schreiben
	uint8_t year_zehner = 0;


	sec_einer = rtc_bufferReceive[0];
	sec_zehner = rtc_bufferReceive[0];
	min_einer = rtc_bufferReceive[1];
	min_zehner = rtc_bufferReceive[1];
	hour_einer = rtc_bufferReceive[2];
	hour_zehner = rtc_bufferReceive[2];

	day_einer = rtc_bufferReceive[3];

	date_einer = rtc_bufferReceive[4];
	date_zehner = rtc_bufferReceive[4];
	month_einer = rtc_bufferReceive[5];
	month_zehner = rtc_bufferReceive[5];
	year_einer = rtc_bufferReceive[6];
	year_zehner = rtc_bufferReceive[6];


	sec_einer &= 15;	//Bits 4,5,6,7 löschen, ergibt Wert vom Einer der Sekunde
	rtc_bufferOut[0] = sec_einer;
	sec_zehner &= 240;	//Bits 0,1,2,3 löschen, ergibt Wert vom Zehner der Sekunde
	rtc_bufferOut[1] = sec_zehner/16;	//Wert vom Zehner wird mit dem Wert 16 skaliert

	min_einer &= 15;
	rtc_bufferOut[2] = min_einer;
	min_zehner &= 240;
	rtc_bufferOut[3] = min_zehner/16;

	hour_einer &= 15;
	rtc_bufferOut[4] = hour_einer;
	hour_zehner &= 48;
	rtc_bufferOut[5] = hour_zehner /= 16;	//anstatt 16 lieber 48?

	day_einer &= 7;
	rtc_bufferOut[6] = day_einer;

	date_einer &= 15;
	rtc_bufferOut[7] = date_einer;
	date_zehner &= 48;
	rtc_bufferOut[8] = date_zehner/16;

	month_einer &= 15;
	rtc_bufferOut[9] = month_einer;
	month_zehner &= 16;
	rtc_bufferOut[10] = month_zehner/16;

	year_einer &= 15;
	rtc_bufferOut[11] = year_einer;
	year_zehner &= 240;
	rtc_bufferOut[12]= year_zehner/16;

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

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_MINUTE ,tmp , 1) ;

	tmp = GET_BCD_HIGH(alarm->hr)  | GET_BCD_LOW(alarm->hr)     ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_HOUR ,tmp , 1) ;

	//set alarm when sec ,mi, and hour matchs

	tmp = rtc_get_reg(RTC_A1_DATE) ;

	//set A1M4 bit to one
	tmp |= 0x80 ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_DATE ,tmp , 1) ;


	//enable rtc alarm
	tmp = rtc_get_reg(RTC_CONTROL_REG) ;

	//set AE1 in control reg
	tmp |= 0x01;

	rtc_write_reg( RTC_I2C_ADDR , RTC_CONTROL_REG ,tmp , 1) ;


}
