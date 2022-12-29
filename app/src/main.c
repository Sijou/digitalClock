/*
 * main.c
 *
 *  Created on: Dec 22, 2022
 *      Author: Lenovo
 */


/***
 *
 *  ADD the fourth column to keypad driver++++++++++++++++++++
 *
 *  //add Write more that 255 byte (LCD)+++++++++++
 *
 *  // LCD +++++++++++++++++++++
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

	 I2C_Init(I2C1) ;

	 Display_Init();	//Configure Display

	 keypad_init(GPIOC , 0) ;


	while(1)
	{
		//Example: print a picture on screen
		Display_Fill(Display_COLOR_WHITE);	//the entire Display is white (written to RAM)
		Display_DrawBitmap(0, 0, helix, 128, 64, Display_COLOR_BLACK);	//data of picture into RAM	(available pictures: helix & looping)
		Display_UpdateScreen();		//all Data written to RAM of Display is printed on display

		char c = keypad_get_pressedkey() ;

		if(c != 0)
		{
			gpio_set_pinState(GPIOA , 5 , HIGH) ;
		}


#ifdef BLINK
		gpio_set_pinState(GPIOA , 5 , HIGH) ;

		delay_ms(500) ;

		gpio_set_pinState(GPIOA , 5 , LOW) ;

		delay_ms(500) ;
#endif /*BLINK*/



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

