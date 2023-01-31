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
 * The function keypad_init() takes two arguments, the first one is the base address of the port to which keypad is connected
 * and the second one is the pin number of the start pin of the keypad.
 * The function initializes the keypad by setting the keypad port and start pin, enabling the clock for the specified GPIO port,
 * and configuring the keypad's key direction (input or output) using the keypad_config_key_direction() function.

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
 *
 *
 *
 * This function configures the direction and pull of pins used for a keypad connected to the given port.
 * The s_pin parameter specifies the starting pin number for the keypad configuration.
 * The function sets the first 4 pins as output with no pull and the next 4 pins as input with pull-up.
 * The function also calls a reset_io(port) function to reset the state of the port's pins, but it's unclear from this code snippet what that function does.
 */



static void keypad_config_key_direction( GPIO_TypeDef * port , int s_pin)
{
	reset_io(port) ;
	//configure the pins as Output with no pull
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//r1  1
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//r2  0
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//r3  0
	gpio_config_pin(port ,s_pin++  ,GPIO_OUT , GPIO_SPEED_LOW , GPIO_NO_PULL,GPIO_PUSHPULL) ;//r4  0
	//configure the pins as Intput with pull-up
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C1  1
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C2  0
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C3  1
	gpio_config_pin(port ,s_pin++ ,GPIO_IN , GPIO_SPEED_LOW , GPIO_PULL_UP,GPIO_PUSHPULL) ;  //C4  1
}

/*
 * This function is used to configure the direction of the pins used for the keypad.
 * The keypad_config_key_direction() function sets the direction of the R pins to output and C pins to input with pull-up,
 * while the keypad_inverse_pin_direction() function sets the direction of the R pins to input with pull-down and C pins to output with no pull.
 * The reset_io() function is used to reset the pin direction and configuration of the specified port.
 * The s_pin argument is used to specify the starting pin number for the keypad pins on the specified port.
 * This is likely to be used in a keypad driver to configure the keypad pins and read keypad input.
 */
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
 * This code is a C function for reading input from a keypad connected to a microcontroller.
 * The function uses the microcontroller's General Purpose Input/Output (GPIO) ports to read the state of the keypad's pins
 * and determine which key, if any, is currently being pressed.
 * The function starts by declaring several variables to store the state of the keypad's pins and the key that was pressed.
 * It then checks the current state of the keypad using a switch statement, with three possible states: State_free, State_key_pressed, and State_key_released.
 * In the State_free case, the function reads the state of the keypad's pins using the IDR register of the GPIOC port,
 * and checks if any key is pressed by comparing the value to the default input state of the keypad. If a key is pressed,
 * the function sets the direction of the keypad's pins to output and uses a loop to determine the row and column of the pressed key.
 * The key's value is then returned.
 * In the State_key_pressed case, the function checks if the key has been released by comparing the state of the keypad's pins to the default input state.
 * If the key has been released, the function sets the state to State_key_released.
 * In the State_key_released case, the function sets the state back to State_free.
 * The function then returns the value of the pressed key, or 0 if no key was pressed.
 * Overall, this function uses a combination of bit shifting, bit masking and loops to read the state of the keypad and determine which key is pressed.
 * The function is not very readable and could be improved by adding more comments and breaking it down into smaller functions for better readability and maintainability.
 */
	char keypad_get_pressedkey()
					{
					    uint16_t porte = 0;   // Set a variable to store the state of the GPIOC pins
					    porte = GPIOC->IDR ; /* IDR is a register of the GPIOC that contains the input states of the pins.
					                            * We can read the whole pin status of the GPIOC port with the IDR register.
					                            */
					    uint8_t  eidr  = 0;  // Set a variable to store the state of the 8 keypad pins
					    uint8_t  ch    = 0;
					    char ret = 0 ;

					    // Check the current state of the keypad
					    switch(state)
					    {
					        case State_free : // Initial state, no key has been pressed yet
					            {
					                eidr = (porte >> start_pin) ; // Get the state of the keypad pins
					                if(eidr != DEFAULT_KEYPAD_PORT_INP) // If the value is different than the default input state
					                {
					                    ch = ~eidr; // Set the inverse of eidr in ch
					                    ch = ch & DEFAULT_KEYPAD_PORT_INP; // Get the row of the pressed key

					                    // Set the first 4 pins as input and the second 4 pins as output
					                    keypad_inverse_pin_direction(keypad_port ,start_pin ) ;
					uint16_t out = (ch << start_pin) ;  //

					keypad_port->ODR = out ;

					delay_ms(1) ;

					porte = keypad_port->IDR ;//get the state of input port

					eidr = (porte >> start_pin) ;//& 0xff ; //get the state of keypad pins
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

