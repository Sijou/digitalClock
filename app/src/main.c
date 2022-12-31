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


#include "display.h"
#include "fonts.h"
#include "logo.h"

volatile uint32_t mtick ;


//#define BLINK
//#define I2C_WRITE_READ_BM


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

	 I2C_Init(I2C2) ;


	 //data = search_address(I2C2) ;

	 uint8_t buff[2] ;
	 buff[0] = 0x09 ;
	 buff[1] = 0x0a ;

	 error = I2C_Write(I2C2 , 0x57 ,buff , 1 ) ;

	 delay_ms(2);

	 I2C_Read(I2C2 ,0x57 , buff , 2) ;

     //128*64
#endif

	 lcd1.dev = I2C1 ;
	 lcd2.dev = I2C2 ;
	 lcd3.dev = I2C3 ;

	 //I2C_Init(I2C1) ;
	 I2C_Init(I2C2) ;
	 I2C_Init(I2C3) ;

	 Display_Init(&lcd3);	//Configure Display
	 Display_Init(&lcd2);	//Configure Display

	 keypad_init(GPIOC , 0) ;
	 //Example: print a picture on screen
	Display_Fill(&lcd3, Display_COLOR_BLACK );	//the entire Display is white (written to RAM)
	Display_DrawBitmap(&lcd3 ,0, 0, looping, 128, 64, Display_COLOR_WHITE);	//data of picture into RAM	(available pictures: helix & looping)
	Display_UpdateScreen(&lcd3);
	Display_Fill(&lcd2, Display_COLOR_WHITE );	//the entire Display is white (written to RAM)
	Display_UpdateScreen(&lcd2);
	while(1)
	{

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

