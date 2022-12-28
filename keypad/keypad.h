/*
 * keypad.h
 *
 *  Created on: Dec 26, 2022
 *      Author: Lenovo
 */




#ifndef KEYPAD_KEYPAD_H_
#define KEYPAD_KEYPAD_H_

#include "gpio.h"
#include "stm32f3xx.h"

typedef enum {
	State_free ,
	State_key_pressed ,
	State_key_released ,

}keypad_states;

void keypad_init(GPIO_TypeDef * port , int s_pin) ;

char keypad_get_pressedkey() ;

#endif /* KEYPAD_KEYPAD_H_ */

