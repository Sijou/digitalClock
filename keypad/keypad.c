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



static void keypad_inverse_pin_direction( GPIO_TypeDef * port , int s_pin) ;

static void keypad_config_key_direction( GPIO_TypeDef * port , int s_pin)  ;

/**
 * config keypad pins
 */
void keypad_init(GPIO_TypeDef * port , int s_pin)
{
	keypad_port = port   ;
	start_pin   = s_pin  ;
	gpio_clock_enable(port) ;
	keypad_config_key_direction(port , s_pin) ;
}

static void keypad_inverse_pin_direction( GPIO_TypeDef * port , int s_pin)
{
	reset_io(port) ;
	gpio_config_pin(port ,s_pin++  ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_DOWN,GPIO_PUSHPULL) ;//R1     0
	gpio_config_pin(port ,s_pin++  ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_DOWN,GPIO_PUSHPULL) ;//R2     1
	gpio_config_pin(port ,s_pin++  ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_DOWN,GPIO_PUSHPULL) ;//R3     0
	gpio_config_pin(port ,s_pin++ ,GPIO_IN  , GPIO_SPEED_LOW , GPIO_PULL_DOWN,GPIO_PUSHPULL) ;//R4     0
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;  //C1     1
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;  //C2     1
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;  //C3     1
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;  //C4     1

}

static void keypad_config_key_direction( GPIO_TypeDef * port , int s_pin)
{
	reset_io(port) ;
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//R1  0
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//R2  0
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//R3  0
	gpio_config_pin(port ,s_pin++ ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//R4   0
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C1  1
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C2  1
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C3  0
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C4  1
}

/**
 * return the pressedkey other wise 0
 */
char keypad_get_pressedkey()
{

	uint16_t porte = 0;
	uint8_t  eidr  = 0;
	uint8_t  ch    = 0;
	char ret = 0 ;

	porte = GPIOC->IDR ;
  // 000 000 0 00 000 0000
	       //  7 >>
	/**
	 *
	 * 0b 0111 0100 0000 1111   //porc
	 * 0b 0000 0111 0100 0000   //7
	 *    &<<
	 *    0000 0000 0111 1111
	 *
	 */
	switch(state)
	{
		case State_free ://initial
			{										//0b00100011111111
				eidr = (porte >> start_pin) ;//  & 0x0000000007f ;  //

				if(eidr != DEFAULT_KEYPAD_PORT_INP)
				{
					ch = ~eidr;
					ch = ch & DEFAULT_KEYPAD_PORT_INP;  //the bit that changed

					keypad_inverse_pin_direction(keypad_port ,start_pin ) ;

					uint16_t out = (ch << start_pin) ;

					keypad_port->ODR = out ;

					delay_ms(1) ;

					porte = keypad_port->IDR ;

					eidr = (porte >> start_pin) & 0xff ;
					//1000
					//port_input = eidr ;//save the state of input port

					int col = 0 ;

					for(col = 0 ; col <4 ; col++)
					{
						/*
						 * 0100 0001
						 * 0000 1111
						 * 0000 0001 &
						 * 0000 0001
						 * 0000 0001
						 */
						if(   (((eidr & 0xf0) >> 4) &  (1<<col)) != 0 )
						{
							break ;
						}
					}

					int row = 0 ;

					for(row = 0 ; row <4 ; row++)
					{
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

