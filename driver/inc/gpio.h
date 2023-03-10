/*
 * gpio.c
 *
 *  Created on: 17 déc. 2022
 *      Author: medali
 */

#ifndef DRIVER_INC_GPIO_H_
#define DRIVER_INC_GPIO_H_


#include "stm32f3xx.h"

#define   GPIOA_CLOCK_ENABLE_BIT      17
#define   GPIOB_CLOCK_ENABLE_BIT	  18
#define   GPIOC_CLOCK_ENABLE_BIT	  19
#define   GPIOD_CLOCK_ENABLE_BIT	  20
#define   GPIOE_CLOCK_ENABLE_BIT	  21
#define   GPIOF_CLOCK_ENABLE_BIT	  22
#define   GPIOG_CLOCK_ENABLE_BIT	  23
#define   GPIOH_CLOCK_ENABLE_BIT	  16
/*#define   GPIOI_CLOCK_ENABLE_BIT	  25
#define   GPIOJ_CLOCK_ENABLE_BIT	  9
#define   GPIOK_CLOCK_ENABLE_BIT	  10*/

/*
 * This code defines several enumerated (enum) types related to configuring and using General Purpose Input/Output (GPIO) pins on a microcontroller.
 * The GPIO_Mode_t enum defines the mode of a GPIO pin, with options for input (GPIO_IN), output (GPIO_OUT), alternate function (GPIO_AF), and analog (GPIO_ANA).
 * The GPIO_Speed_t enum defines the speed of a GPIO pin, with options for low, medium, high, and very high speeds.
 * The GPIO_Pull_t enum defines the pull-up or pull-down resistors on a GPIO pin, with options for pull-up (GPIO_PULL_UP), pull-down (GPIO_PULL_DOWN), and no pull (GPIO_NO_PULL).
 * The GPIO_out_type_t enum defines the output type of a GPIO pin, with options for open drain (GPIO_OPENDRAIN) or push-pull (GPIO_PUSHPULL).
 * The Pin_State enum defines the state of a GPIO pin, with options for high (HIGH) or low (LOW).
 * These enums are used to configure the behavior and operation of the GPIO pins on the microcontroller, such as setting the pin as an input or output,
 * setting the pin speed and pull resistors, and setting the output type for output pins.
 *
 */

typedef enum {
	GPIO_IN   =  0x00 ,
	GPIO_OUT  =  0x01 ,
	GPIO_AF   =  0x02 ,
	GPIO_ANA  =  0x03 ,
}GPIO_Mode_t ;

typedef enum {
	GPIO_SPEED_LOW  	  = 0x00 ,
	GPIO_SPEED_MEDUIM	  = 0x01 ,
	GPIO_SPEED_HIGH 	  = 0x02 ,
	GPIO_SPEED_VERY_HIGH  = 0x03 ,
}GPIO_Speed_t;

typedef enum {
	GPIO_PULL_UP   = 0x01 ,
	GPIO_PULL_DOWN = 0x02 ,
	GPIO_NO_PULL   = 0x00 ,
}GPIO_Pull_t;


typedef enum
{
	GPIO_OPENDRAIN = 1,
	GPIO_PUSHPULL  = 0,
}GPIO_out_type_t;

typedef enum {
	HIGH ,
	LOW
}Pin_State;

void gpio_clock_enable(GPIO_TypeDef * Port) ;

void gpio_config_pin(GPIO_TypeDef * Port , int Pin , GPIO_Mode_t Mode ,
					   GPIO_Speed_t speed ,GPIO_Pull_t pull, GPIO_out_type_t otype) ;

void gpio_set_pinState(GPIO_TypeDef * Port , int Pin , Pin_State State)  ;

void gpio_toggel(GPIO_TypeDef * Port , int Pin ) ;

void reset_io(GPIO_TypeDef * Port );

#endif /* DRIVER_INC_GPIO_H_ */
