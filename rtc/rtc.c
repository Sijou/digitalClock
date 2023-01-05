#include "stm32f303xe.h"
#include "i2c.h"
#include "rtc.h"

I2C_TypeDef * rtc_i2c ;




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


static uint8_t rtc_get_reg(uint8_t reg)
{
	uint8_t val ;

	I2C_Write(I2C1 , 0x68 , reg , 1) ;

	I2C_Read(I2C1 , 0x68 , &val , 1) ;

	return val ;
}

void rtc_get_time(rtc_time_t * rtc_time )
{
	rtc_time->sec  = rtc_get_reg(RTC_REG_SECOND) ;
	rtc_time->min  = rtc_get_reg(RTC_REG_MINUTE) ;
	rtc_time->hr   = rtc_get_reg(RTC_REG_HOUR) ;
}
