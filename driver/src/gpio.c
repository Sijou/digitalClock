/*
 * gpio.c
 *
 *  Created on: 17 déc. 2022
 *      Author: medali
 */

#include "stm32f303xe.h"
#include "gpio.h"
/*
 * After each device reset, all peripheral clocks are disabled (except for the SRAM and FLITF).
Before using a peripheral user has to enable its clock in the RCC_AHBENR,
RCC_APB2ENR or RCC_APB1ENR register.  s.50
 */
void gpio_clock_enable(GPIO_TypeDef * Port)
{
	if(Port == GPIOA)        //  Bit 17 IOPAEN: I/O port A clock enable
	{
		RCC->AHBENR |= (1 << GPIOA_CLOCK_ENABLE_BIT) ; //(1<<17);
	}
	else if(Port == GPIOB)  //    Bit 18 IOPBEN: I/O port B  clock enable
	{
		RCC->AHBENR |= (1 << GPIOB_CLOCK_ENABLE_BIT) ;
	}
	else if(Port == GPIOC)   //  Bit 19 IOPCEN: I/O port C clock enable
	{
		RCC->AHBENR |= (1 << GPIOC_CLOCK_ENABLE_BIT) ;
	}
	else if(Port == GPIOD)    //  Bit 20 IOPDEN: I/O port D clock enable
	{
		RCC->AHBENR |= (1 << GPIOD_CLOCK_ENABLE_BIT) ;
	}
	else if(Port == GPIOE)    //  Bit 21 IOPEEN: I/O port E clock enable
	{
		RCC->AHBENR |= (1 << GPIOE_CLOCK_ENABLE_BIT) ;
	}
	else if(Port == GPIOF)   //  Bit 22 IOPFEN: I/O port F clock enable
	{
		RCC->AHBENR |= (1 << GPIOF_CLOCK_ENABLE_BIT) ;
	}
	else if(Port == GPIOG)    //  Bit 23 IOPGEN: I/O port G clock enable
	{
		RCC->AHBENR |= (1 << GPIOG_CLOCK_ENABLE_BIT) ;
	}
	else if(Port == GPIOH)    //  Bit 16 IOPHEN: I/O port H clock enable
	{
		RCC->AHBENR |= (1 << GPIOH_CLOCK_ENABLE_BIT) ;
	}
	/*else if(Port == GPIOI)
	{
		RCC->AHBENR |= (1 << GPIOI_CLOCK_ENABLE_BIT) ;
	}*/
	/*
	else if(Port == GPIOJ)
	{
		RCC->AHB1ENR |= (1 << GPIOJ_CLOCK_ENABLE_BIT) ;
	}
	else if(Port == GPIOK)
	{
		RCC->AHB1ENR |= (1 << GPIOK_CLOCK_ENABLE_BIT) ;
	}
	*/
	else{

	}
}

void reset_io(GPIO_TypeDef * Port ) {
	Port->MODER   = 0x00 ;             // set pin Mode
	Port->OTYPER  = 0x00 ;             //set output type OD/PP
	Port->OSPEEDR = 0x00 ;             //set output speed
	Port->PUPDR	  = 0x00 ;             //set pin Pull
}

void gpio_config_pin(GPIO_TypeDef * Port , int Pin , GPIO_Mode_t Mode ,
					   GPIO_Speed_t speed ,GPIO_Pull_t pull, GPIO_out_type_t otype)
{
	Port->MODER   |= (Mode  << 2 * Pin) ; // set  pin Mode
	Port->OTYPER  |= (otype << Pin)     ; //set output type OD/PP
	Port->OSPEEDR |= (speed << 2 * Pin) ; //set output speed
	Port->PUPDR	  |= (pull  << 2 * Pin) ; //set pin Pull
}

void gpio_set_pinState(GPIO_TypeDef * Port , int Pin , Pin_State State)
{
	//assert pin < 16
	if(State == HIGH)
	{
		Port->ODR |= (1 << Pin) ;
	}
	else {
		Port->ODR &= ~(1 << Pin) ;
	}
}
