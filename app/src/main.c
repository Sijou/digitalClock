/*
 * main.c
 *
 *  Created on: Dec 22, 2022
 *      Author: Lenovo
 */


/***
 *

 *
 *  //
 */
#include "stm32f303xe.h"
#include "i2c.h"
#include "gpio.h"
#include "main.h"
#include "keypad.h"

#include "stdio.h"
#include "display.h"
#include "fonts.h"
#include "logo.h"
#include "alarm_logo.h"
#include "rtc.h"
#include "test1.h"
#include "test2.h"
#include "test3.h"
#include "test4.h"

volatile uint32_t mtick ;


//#define BLINK
//#define I2C_WRITE_READ_BM

int rtc_I2C_Read1 ( uint8_t address, uint8_t dataRegister, uint8_t  *bufferReceive, uint8_t rcvDataLen)
{
/*
1. send slave address + lenght of Data (in Byte) + WRITE
2. wait for ACK
3. send the Data (of register to read from)
4. wait for ACK
5. Restart Condition
6. send slave address + lenght of Data (in Byte) + READ
7. wait for ACK
8. read the data (from the register)
9.wait for ACK
10.repeat 9./10. if more data to read
*/

	int remainingRX = rcvDataLen;

	I2C1->CR2 = ((address << 1) << I2C_CR2_SADD_Pos) |(1 << I2C_CR2_NBYTES_Pos);	//send address + data of register + WRITE

	I2C1->CR2 |= I2C_CR2_START;		// Generate START

	while ((I2C1->ISR & I2C_ISR_TXIS)==0) {	//abfrage am TXIS bit, is set to 1 when TXDR is empty and should be 1 at this point
		if ((I2C1->ISR & I2C_ISR_NACKF)){
			return -1;
		}
	}

	I2C1->TXDR = dataRegister;	//TXDR get the data to transmit, - TXIS is set to 0

	while ((I2C1->ISR & I2C_ISR_TC)==0) {	//when NBYTES has been send, is cleared when Start/Stop bit is set
		if (I2C1->ISR & I2C_ISR_NACKF)
			return -2;
	}

	I2C1->CR2 |= I2C_CR2_STOP;

	I2C1->CR2 = (((address << 1) << I2C_CR2_SADD_Pos) |(1 << I2C_CR2_NBYTES_Pos) | (1 << I2C_CR2_RD_WRN_Pos));	//send address + anz of bytes to read + READ

	I2C1->CR2 |= I2C_CR2_START;		// Generate START

	while ((I2C1->ISR & I2C_ISR_RXNE) == 0) {
		if (I2C1->ISR & I2C_ISR_NACKF)
			return -1;
	}

	bufferReceive[dataRegister] = I2C1->RXDR;

	I2C1->CR2 |= I2C_CR2_NACK;
	I2C1->CR2 |= I2C_CR2_STOP;

	return 0 ;
}


uint8_t data ;
uint32_t SystemCoreClock_ = 8000000; //8 Mhz


uint8_t data = 0 ;
uint8_t i = 0 ;
int error ;
OLED_t lcd1 ;
OLED_t lcd2 ;
OLED_t lcd3 ;

int main()
{

  	while ((RCC->CR & 2) == 0);  //wait until HSI is Ready

	systick_enable() ;

	RCC->CFGR &= ~(1<<7)  ; //reset HPRE4 The AHB clock frequency == sys clk
	RCC->CFGR &= ~(1<<12) ; // APB1 clock no division
	RCC->CFGR &= ~(1<<15) ; // APB2 clock no division

	gpio_clock_enable(GPIOA) ;

	gpio_config_pin(GPIOA,5,GPIO_OUT,GPIO_SPEED_LOW,GPIO_NO_PULL ,GPIO_PUSHPULL);

	gpio_set_pinState(GPIOA , 5 , LOW) ;

	//SYSCFG->CFGR1 |= (1<<16) |(1<<17) ;// set PB6 and PB7 I2C mode
#ifdef I2C_WRITE_READ_BM

	 uint8_t buff[3] ;

	 I2C_Init(I2C1) ;


	 data = search_address(I2C1 , buff) ;

#endif

	 lcd1.dev = I2C1 ;
	 lcd2.dev = I2C2 ;
	 lcd3.dev = I2C3 ;

	 I2C_Init(I2C1) ;
	 I2C_Init(I2C2) ;
	 I2C_Init(I2C3) ;


	 uint8_t buff1[5] = {0};
	 buff1[4] = 0x91 ;
	 buff1[0] = RTC_REG_MINUTE ;




	 I2C_Write(I2C1 , 0x68 , buff1 , 1) ;
	 I2C_Read(I2C1 , 0x68 , buff1 , 1) ;



	 Display_Init(&lcd3);	//Configure Display
	 Display_Init(&lcd2);	//Configure Display
	 Display_Init(&lcd1);	//Configure Display
	 keypad_init(GPIOC , 0) ;
	 //Example: print a picture on screen
	Display_Fill(&lcd3, Display_COLOR_BLACK);	//the entire Display is white (written to RAM)
	Display_DrawBitmap(&lcd3 ,0, 0, image_data, 128, 64, Display_COLOR_WHITE);	//data of picture into RAM	(available pictures: helix & looping)
	Display_UpdateScreen(&lcd3);

	Display_Fill(&lcd2, Display_COLOR_WHITE );	//the entire Display is white (written to RAM)
	Display_UpdateScreen(&lcd2);

	Display_Fill(&lcd1, Display_COLOR_BLACK );	//the entire Display is white (written to RAM)
	Display_DrawBitmap(&lcd1 ,0, 0, alarm_logo, 128, 64, Display_COLOR_WHITE);	//data of picture into RAM	(available pictures: helix & looping)
	Display_UpdateScreen(&lcd1);
//
//	uint8_t in_buffer[20]  ;
//	uint8_t out_buffer[20] ;
//
/**  bug in one of those functions**/
//	rtc_I2c_ReadAll(in_buffer);
//	rtc_I2C_Calculate(in_buffer , out_buffer);
//	Display_printTime(&lcd3 , out_buffer,&Font_7x10 );
//	Display_UpdateScreen(&lcd3);
	rtc_time_t t ;

	char my_time[20] ;
	while(1)
	{
		rtc_get_time(&t) ;

		sprintf(my_time , "%d:%d:%d/0",t.hr , t.min , t.sec) ;


		Display_GotoXY(&lcd2 , 20,5);
		Display_Puts(&lcd2 ,my_time ,& Font_7x10 , Display_COLOR_BLACK ) ;
		Display_UpdateScreen(&lcd2);
		char c = keypad_get_pressedkey() ;

		if(c != 0)
		{
			gpio_set_pinState(GPIOA , 5 , HIGH) ;
			Display_Putc(&lcd2 , c , &Font_7x10 , Display_COLOR_BLACK) ;
			Display_UpdateScreen(&lcd2);
		}


#ifdef BLINK
		gpio_set_pinState(GPIOA , 5 , HIGH) ;

		delay_ms(500) ;

		gpio_set_pinState(GPIOA , 5 , LOW) ;

		delay_ms(500) ;
#endif /*BLINK*/


		/** width 7* height 10
		 * 0x3800, 0x4400, 0x4000, 0x3000, 0x0800, 0x0400, 0x4400, 0x3800, 0x0000, 0x0000,  // S
		 *
		 *   0 0 1 1 1 0 0 0
		 *   0 1 0 0 0 1 0 0
		 *   0 1 0 0 0 0 0 0
		 *   0 0 1 1 0 0 0 0
		 *   0 0 0 0 1 0 0 0
		 *   0 0 0 0 0 1 0 0
		 *   0 1 0 0 0 1 0 0
		 *   0 0 1 1 1 0 0 0
		 *   0 0 0 0 0 0 0 0
		 *   0 0 0 0 0 0 0 0
		 *
		 * 	 0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x5000, 0x2000, 0x0000,  // v
		 *
		 * 	 0 0 0 0 0 0 0 0
		 * 	 0 0 0 0 0 0 0 0
		 * 	 1 0 0 0 1 0 0 0
		 * 	 1 0 0 0 1 0 0 0
		 * 	 1 0 0 0 1 0 0 0
		 *   0 1 0 1 0 0 0 0
		 *   0 0 1 0 0 0 0 0
		 * 	 0 0 0 0 0 0 0 0
		 */


	}
}


void systick_enable()
{
	SysTick->LOAD  = (SystemCoreClock_/1000) - 1;                  /* set reload register */
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Systick Interrupt */
	SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
					 SysTick_CTRL_TICKINT_Msk   |
					 SysTick_CTRL_ENABLE_Msk;
}


void SysTick_Handler()
{
  mtick++;
}

void delay_ms(uint32_t ms)
{
  uint32_t st = get_mtick();

  while( get_mtick() - st <ms);
}

uint32_t get_mtick()
{
  return mtick;
}

