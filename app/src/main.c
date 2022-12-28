/*
 * main.c
 *
 *  Created on: Dec 22, 2022
 *      Author: Lenovo
 */


/***
 *
 *  ADD the fourth column to keypad driver
 *
 *  add Write more that 255 byte (LCD)
 *
 *  LCD  ,( UART )
 *
 */
#include "stm32f303xe.h"
#include "i2c.h"
#include "gpio.h"
#include "main.h"
#include "keypad.h"

volatile uint32_t mtick ;

#define I2C_WRITE_READ_BM
uint8_t data ;
uint32_t SystemCoreClock_ = 8000000; //8 Mhz


uint8_t data = 0 ;
uint8_t i = 0 ;
int error ;

int main()
{

	while ((RCC->CR & 2) == 0); //wait until HSI is Ready

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


	// data = search_address(I2C2) ;

	 uint8_t buff[2] ;
	 buff[0] = 0x09 ;
	 buff[1] = 0x0a ;

	 error = I2C_Write(I2C2 , 0x57 ,buff , 1 ) ;

	 delay_ms(2);

	 I2C_Read(I2C2 ,0x57 , buff , 2) ;


     //128*64
#endif

	 keypad_init(GPIOC , 0) ;
//	 uint16_t porte = 0;
//	 uint8_t  eidr  = 0;
//	 porte = GPIOC->IDR ;
//
//	 eidr = (porte >> 0) & 0x7f ;

	while(1)
	{

		char c = keypad_get_pressedkey() ;




//		uint16_t key = (GPIOC->IDR )&0x007f ;
//		key = (key >> 4 ) & 0x07;
//		if(key != 7)
		if(c != 0)
		{
			gpio_set_pinState(GPIOA , 5 , HIGH) ;
		}
//		gpio_set_pinState(GPIOA , 5 , HIGH) ;
//
//		delay_ms(500) ;
//
//		gpio_set_pinState(GPIOA , 5 , LOW) ;
//
//		delay_ms(500) ;
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


/**
 *
 * Exemple transmmiting 2 byte
 *
 * I2C1->CR2  = ((data << 1) << I2C_CR2_SADD_Pos) ;  // set the slave address
	 I2C1->CR2 |=   (2 << I2C_CR2_NBYTES_Pos);		   // set the number of bytes to transmitted
	 I2C1->CR2 |= I2C_CR2_START; 					   // generate the start condition
	 while ((I2C1->ISR & I2C_ISR_TXIS) == 0)  // wait for ASK
	 {
		  if (I2C1->ISR & I2C_ISR_NACKF)	      // No ASK is received (error)
			  while(1) ;
	 }


	 I2C1->TXDR = 0x09;
	 while ((I2C1->ISR & I2C_ISR_TXIS) == 0) {
		 if (I2C1->ISR & I2C_ISR_NACKF)
			 while(1) ;
	 }


	 I2C1->TXDR = 0x09;
	 	 while ((I2C1->ISR & I2C_ISR_TC) == 0) {
	 		 if (I2C1->ISR & I2C_ISR_NACKF)
	 			 while(1) ;
	 	 }

	 I2C1->CR2 |= (1<<I2C_CR2_STOP_Pos); //Generate Stop condition
 */
