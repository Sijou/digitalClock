/*
 * gpio.c
 *
 *  Created on: 17 déc. 2022
 *      Author: medali
 */

#include "stm32f303xe.h"
#include "gpio.h"
/*This is a C function that enables the clock for a specific GPIO port on a microcontroller.
 * The function takes in a pointer to a GPIO_TypeDef structure as an input, which represents the port that needs to have its clock enabled.
 * The function then checks the value of the pointer and compares it to the memory addresses of the various GPIO ports (GPIOA, GPIOB, etc.).
 * If the pointer matches the address of a specific port, the corresponding clock enable bit in the RCC->AHBENR register is set,
 * which enables the clock for that port.
 * The function also contains comments indicating which bit corresponds to which port.
 * The function also include the provision for ports like GPIOI, GPIOJ and GPIOK but they are commented out.
 *
 *
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
/*
 * The function reset_io takes a pointer to a GPIO_TypeDef struct as an argument.
 * This struct represents a General-Purpose Input/Output (GPIO) peripheral on a microcontroller.
 * The function sets the mode, output type, output speed, and pull-up/pull-down settings of all the pins on the specified GPIO port to their reset values.
 * The MODER register controls the pin mode (input or output),
 * the OTYPER register controls the output type (open-drain or push-pull),
 * the OSPEEDR register controls the output speed,
 * and the PUPDR register controls the pull-up/pull-down settings.
 * By setting all of these registers to zero, the function effectively disables all the pins on the specified port
 * and sets them to their default state, which is an input mode with no pull-up/pull-down resistors and no output buffer.
 */
void reset_io(GPIO_TypeDef * Port ) {
	Port->MODER   = 0x00 ;             // set pin Mode
	Port->OTYPER  = 0x00 ;             //set output type OD/PP
	Port->OSPEEDR = 0x00 ;             //set output speed
	Port->PUPDR	  = 0x00 ;             //set pin Pull
}

/*
 * This function configures a specific pin on a given GPIO port (specified by the Port variable) with the specified parameters.
 * The Pin variable specifies which pin on the port to configure.
 * The Mode variable sets the pin's mode (input, output, alternate function, etc.).
 * The speed variable sets the pin's output speed (low, medium, or high).
 * The pull variable sets the pin's pull-up or pull-down resistance.
 * And the otype variable sets the pin's output type (open-drain or push-pull).
 * The function sets these parameters by setting the appropriate bits in the MODER, OTYPER, OSPEEDR, and PUPDR registers of the specified port.
 */
void gpio_config_pin(GPIO_TypeDef * Port , int Pin , GPIO_Mode_t Mode ,
					   GPIO_Speed_t speed ,GPIO_Pull_t pull, GPIO_out_type_t otype)
{
	Port->MODER   |= (Mode  << 2 * Pin) ; // set  pin Mode
	Port->OTYPER  |= (otype << Pin)     ; //set output type OD/PP
	Port->OSPEEDR |= (speed << 2 * Pin) ; //set output speed
	Port->PUPDR	  |= (pull  << 2 * Pin) ; //set pin Pull
}


/*
 * This function sets the state of a specified pin on a specified GPIO port.
 * The function takes in three parameters: the GPIO port (of type GPIO_TypeDef), the pin number (an int), and the desired state of the pin (of type Pin_State).
 * First it checks if the pin number is less than 16, this is to ensure that the pin number is within the valid range.
 * Then it uses an if else loop to check if the state is high or low,
 * if it is high it uses bitwise OR operator to set the specified pin to high,
 * if it is low it uses bitwise AND operator with NOT operator to set the specified pin to low.
 */

void gpio_set_pinState(GPIO_TypeDef * Port , int Pin , Pin_State State)
{
    // assert that the pin number is less than 16
    // this is to ensure that the pin number is within the valid range
    // assert(pin < 16);

    if(State == HIGH)
    {
        // Use bitwise OR operator to set the specified pin to high
        Port->ODR |= (1 << Pin);
    }
    else {
        // Use bitwise AND operator with NOT operator to set the specified pin to low
        Port->ODR &= ~(1 << Pin);
    }
}

/*
 * This function toggles the state of a specified pin on a specified GPIO port.
 * The function takes in two parameters: the GPIO port (of type GPIO_TypeDef), and the pin number (an int).
 * The function first reads the current state of the pin using the IDR register,
 * then using bitwise operator it applies a mask on the IDR register to get the state of the specific pin.
 * Then it inverts the state of the pin using bitwise NOT operator and writes it back to the ODR register to change the state of the pin.
 * It basically reads the current state of the pin, inverts it, and then writes the new state to the pin.
 */

void gpio_toggel(GPIO_TypeDef * Port , int Pin )
{
    // Read the current state of the specified pin using the IDR register
    uint16_t pstate = Port->IDR ;

    // Create a mask to isolate the specified pin
    uint16_t pin_mask = 0x0001 << Pin ;

    // Apply the mask to the current state to get the state of the specified pin
    pstate &= pin_mask ;

    // Invert the state of the specified pin using bitwise NOT operator
    pstate = ~pstate ;

    // Write the new state to the ODR register to change the state of the pin
    Port->ODR = pstate ;
}
