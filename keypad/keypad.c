/*
 * keypad.c
 *
 *  Created on: Dec 26, 2022
 *      Author: Lenovo
 */


/*
 * keypad.c
 *
 *  Created on: 25 déc. 2022
 *      Author: medali
 */

#include "keypad.h"
#include "gpio.h"
#include "main.h"

char keys[4][4] = {{'1','2','3' ,'A'} ,{'4','5','6','B'} ,{'7','8','9','C'},{'*','0','#','D'} } ;
GPIO_TypeDef * keypad_port ;
int start_pin ;
keypad_states state = State_free ;


#define  DEFAULT_KEYPAD_PORT_INP    0xf0

static void keypad_config_key_direction( GPIO_TypeDef * port , int s_pin)  ;
static void keypad_inverse_pin_direction( GPIO_TypeDef * port , int s_pin) ;

/**
 * config keypad pins
 * In order for the microcontroller to determine which key is pressed, it must first pull each of the four columns (pins 1-4)
 * either low or high and then query the states of the four rows (pins 5-8).
 * Depending on the state of the columns, the microcontroller can determine which key was pressed.
 * Let's assume your program switches all four columns to low and then switches the first row to high.
 * Then it reads the input states of each column and reads pin 1 high.
 * This means that a contact has been made between column 4 and row 1, so the 'A' key has been pressed.
 */
void keypad_init(GPIO_TypeDef * port , int s_pin)
{
	keypad_port = port   ;
	start_pin   = s_pin  ;
	gpio_clock_enable(port) ;
	keypad_config_key_direction(port , s_pin) ;////set  the first 4 pins as output and the second 4 pins as input
}
/*
 * rows (pins : 1-4)
 * columns (pins : 5-8)
 */



static void keypad_config_key_direction( GPIO_TypeDef * port , int s_pin)
{
	reset_io(port) ;
	//configure the pins as Output with no pull
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//c1  1
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//c2  0
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//c3  0
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//c4  0
	//configure the pins as Intput with pull-up
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //r1  1
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //r2  0
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //r3  1
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C4  1
}

static void keypad_inverse_pin_direction( GPIO_TypeDef * port , int s_pin)
{
	reset_io(port) ;
	//configure the pins as Intput with pull-down
	gpio_config_pin(port ,s_pin++  ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_DOWN,GPIO_PUSHPULL) ;//R1     0
	gpio_config_pin(port ,s_pin++  ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_DOWN,GPIO_PUSHPULL) ;//R2     1
	gpio_config_pin(port ,s_pin++  ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_DOWN,GPIO_PUSHPULL) ;//R3     1
	gpio_config_pin(port ,s_pin++ ,GPIO_IN  , GPIO_SPEED_LOW , GPIO_PULL_DOWN,GPIO_PUSHPULL) ;//R4     1
	//configure the pins as Output with no pull
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;  //C1     0
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;  //C2     1
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;  //C3     0
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;  //C4     0

}



/**
 * return the pressedkey other wise 0
 */
char keypad_get_pressedkey()
{

	uint16_t porte = 0;   // set porte to zero where the pins of the used GPIO port (C , Pins : 0-15) will be stored.
	porte = GPIOC->IDR ; /* IDR is one of the GPIO registers that contains the input states of the pins of a GPIO port.
	                      * We can read the whole pin status of a GPIO port with the IDR register.
	                      */
    uint8_t  eidr  = 0;  // set eidr to zero where only the 8 used pins will be stored.
	uint8_t  ch    = 0;
	char ret = 0 ;

    switch(state)
	{
		case State_free ://initial
			{
				//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
				// x x  x  x  x  x                  x x
				eidr = (porte >> start_pin) ;//  & 0xff ; // get the state od the keypad pins
                 /*
                  * with the add 0xff we will set the rest of the pins we don't need to 0 but here eidr is uint8_t anyway
                  * so only the first 8 pins are assumed here.
                  * and adding 0xff is then no longer necessary
                  */
				if(eidr != DEFAULT_KEYPAD_PORT_INP)       // if now the value is different than the DEFAULT_KEYPAD_PORT_INP 0xf0
				{
					ch = ~eidr;                           // set the inverse of eidr in ch
					ch = ch & DEFAULT_KEYPAD_PORT_INP;    // with the addition of 0xf0 we get 1 in the changed bit position
                                                            // we get the used row


					//set  the first 4 pins as input and the second 4 pins as output
					keypad_inverse_pin_direction(keypad_port ,start_pin ) ;

					uint16_t out = (ch << start_pin) ;  //

					keypad_port->ODR = out ;

					delay_ms(1) ;

					porte = keypad_port->IDR ;//get the state of input port

					eidr = (porte >> start_pin) & 0xff ; //get the state of keypad pins
					//port_input = eidr ;

					int col = 0 ;

					for(col = 0 ; col <4 ; col++)
					{
						/* 0100 0001
						 * 1111 0000 0xf0 &
						 * 0100 0000
						 */
						if(   (((eidr & 0xf0) >> 4) &  (1<<col)) != 0 )
						{
							break ;
						}
					}

					int row = 0 ;

					for(row = 0 ; row <4 ; row++)
					{   /*
					 * 0100 0001
					 * 0000 1111 0x0f &
					 * 0000 0001
					 */
						if( ((eidr & 0x0f) & (1<<row)) != 0)
						{
							break ;
						}
					}

					//reset the direction

					ret = keys[row][col] ;

					keypad_config_key_direction(keypad_port ,start_pin ) ;

					state = State_key_pressed ;

				}
				else
				{
					//nope
				}
				break ;
			}
		case State_key_pressed :
			{
				eidr = (porte >> start_pin) & 0xff ;

				if(eidr == DEFAULT_KEYPAD_PORT_INP)
				{
					state = State_key_released ;
				}
				break ;
			}
		case State_key_released :
			{

				state = State_free ;
				break ;
			}
		default :
			break ;

	}


	return ret ;
}

