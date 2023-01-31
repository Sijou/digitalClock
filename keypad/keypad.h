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
/*
 * This is a definition of an enumerated data type called "keypad_states." Enumerated data types are used to define a set of named integer values.
 * In this case, the enumerated type "keypad_states" has three values: State_free, State_key_pressed, and State_key_released.
 * These values can be used to represent the possible states of a keypad, such as whether a key is currently pressed, released, or in a neutral state.
 */
typedef enum {
	State_free ,
	State_key_pressed ,
	State_key_released ,

}keypad_states;

void keypad_init(GPIO_TypeDef * port , int s_pin) ;

char keypad_get_pressedkey() ;

#endif /* KEYPAD_KEYPAD_H_ */

