/*
 * main.c
 *
 *  Created on: Dec 22, 2022
 *      Author: Lenovo
 */


/***
 *

 *
 *  //
 */
#include "stm32f303xe.h"

#include "i2c.h"
#include "gpio.h"
#include "main.h"
#include "keypad.h"

#include "stdio.h"
#include "display.h"
#include "fonts.h"
#include "logo.h"
#include "alarm_logo.h"
#include "rtc.h"
#include "test1.h"
#include "test2.h"
#include "test3.h"
#include "test4.h"

volatile uint32_t mtick ;


//#define BLINK
//#define I2C_WRITE_READ_BM
/*
int rtc_I2C_Read1 ( uint8_t address, uint8_t dataRegister, uint8_t  *bufferReceive, uint8_t rcvDataLen)
{

1. send slave address + lenght of Data (in Byte) + WRITE
2. wait for ACK
3. send the Data (of register to read from)
4. wait for ACK
5. Restart Condition
6. send slave address + lenght of Data (in Byte) + READ
7. wait for ACK
8. read the data (from the register)
9.wait for ACK
10.repeat 9./10. if more data to read
**/



uint32_t SystemCoreClock_ = 8000000; //8 Mhz


//uint8_t data = 0 ;
//uint8_t i = 0 ;
int error ;
OLED_t lcd1 ;
OLED_t lcd2 ;
OLED_t lcd3 ;

OLED_t current_lcd ;


const char * MENU[] = { "time" , "date" , "Parameter"} ;

const char * SUB_MENU_TIME[] = {"hour config","minute config","second config"};
const char * SUB_MENU_DATE[] = {"day config","month config","year config"};
const char * SUB_MENU_PARM[] = {"alarm1 config","alarm2 config","buzze config" , "night mode"};

const int submenu_item_lenght[] = {3,3,4} ;
const char ** SUB_MENU[] =  {SUB_MENU_TIME , SUB_MENU_DATE , SUB_MENU_PARM } ;

GPIO_TypeDef *  LED_PORT = GPIOA ;

int LED_TAB[3] = { 6 , 11 , 12 } ;

/*
 * The function my_alarm1_callback is a callback function that is called when an alarm event occurs.
 * In this specific implementation, it sets the state of GPIO pin A5 to high.
 * This could be used, for example, to turn on an LED or trigger some other action in response to the alarm event.
 * The exact behavior of the function may depend on how it is used within the larger program and what the intended purpose of the alarm is.
 */
void my_alarm1_callback()
{
	//

	gpio_set_pinState(GPIOA , 5 , HIGH) ;

	//
}



int main()
{
	rtc_time_t t ,tm;
	rtc_date_t d ;

  	while ((RCC->CR & 2) == 0);  // RCC Clock control register (RCC_CR) , wait until HSI is Ready

	systick_enable() ;
    //Clock configuration register (RCC_CFGR)
	RCC->CFGR &= ~(1<<7)  ; //reset HPRE4 The AHB clock frequency == sys clk
	RCC->CFGR &= ~(1<<12) ; // APB1 clock no division
	RCC->CFGR &= ~(1<<15) ; // APB2 clock no division

	gpio_clock_enable(GPIOA) ;

	gpio_config_pin(GPIOA,5,GPIO_OUT,GPIO_SPEED_LOW,GPIO_NO_PULL ,GPIO_PUSHPULL);

	gpio_set_pinState(GPIOA , 5 , LOW) ;


	gpio_config_pin(LED_PORT, LED_TAB[0] ,GPIO_OUT,GPIO_SPEED_LOW,GPIO_NO_PULL ,GPIO_PUSHPULL);
	gpio_config_pin(LED_PORT, LED_TAB[1] ,GPIO_OUT,GPIO_SPEED_LOW,GPIO_NO_PULL ,GPIO_PUSHPULL);
	gpio_config_pin(LED_PORT, LED_TAB[2] ,GPIO_OUT,GPIO_SPEED_LOW,GPIO_NO_PULL ,GPIO_PUSHPULL);




	lcd1.dev = I2C1 ;
	lcd2.dev = I2C2 ;
	lcd3.dev = I2C3 ;

	I2C_Init(I2C1) ;
	I2C_Init(I2C2) ;
	I2C_Init(I2C3) ;

//	 d.day  = 31 ;
//	 d.month = 1 ;
//	 d.year = 23 ;
//
//	 t.hr = 19 ;
//	 t.min = 31 ;
//	 t.sec = 30 ;
//	 t.mode = H_24 ;


	 rtc_init(I2C1);

//	 rtc_set_date( &d);
//	 rtc_set_time(&t) ;

	 tm.hr = 19;
	 tm.min = 15 ;
	 tm.sec = 00 ;
	 rtc_set_alarm(&tm) ;

	 rtc_set_alarm_callback(&my_alarm1_callback) ;

	 Display_Init(&lcd3);	//Configure Display
	 Display_Init(&lcd2);	//Configure Display
	 Display_Init(&lcd1);	//Configure Display
	 keypad_init(GPIOC , 0) ;

	 char key = 0 ;

	system_state_t sys_state = state_init;
	idle_sub_state  idel_sub_state = state_display_date ;
	menu_nav_sub_state_t menu_nav_sub_state = menu_nav_start;


	uint32_t start_timer = get_mtick();
	uint32_t state_idle_timer = 0 ;
	uint32_t led_timer  = 0 ;

	Display_Fill(&lcd1, Display_COLOR_BLACK);
	Display_Fill(&lcd2, Display_COLOR_BLACK);
	Display_Fill(&lcd3, Display_COLOR_BLACK);

	Display_DrawBitmap(&lcd1 ,0, 0, titel_image, 128, 64, Display_COLOR_WHITE);
	Display_DrawBitmap(&lcd2 ,0, 0, titel_image, 128, 64, Display_COLOR_WHITE);
	Display_DrawBitmap(&lcd3 ,0, 0, titel_image, 128, 64, Display_COLOR_WHITE);

	Display_UpdateScreen(&lcd1);
	Display_UpdateScreen(&lcd2);
	Display_UpdateScreen(&lcd3);


	gpio_set_pinState(GPIOA , LED_TAB[0]  , LOW) ;
	gpio_set_pinState(GPIOA , LED_TAB[1]  , LOW) ;
	gpio_set_pinState(GPIOA , LED_TAB[2]  , LOW) ;


	while(1)
	{

		switch(sys_state)
		{
			case state_init:
			{
				if(get_mtick() - start_timer > 3000)
				{
					Display_Clear(&lcd1) ;
					Display_Clear(&lcd2) ;
					Display_Clear(&lcd3) ;
					sys_state = state_idle ;  //trans
					state_idle_timer = get_mtick() +1000;
				}

				break ;
			}

			case state_idle:
			{
				static uint32_t sub_state_timer = 0 ;

				switch(idel_sub_state)
				{

					case state_display_time :
					{
						if(get_mtick() - state_idle_timer >= 960)
						{
							///
							rtc_get_time(&t) ;
							char hour[4] ;
							char minute[4] ;
							char second[4] ;

							sprintf(hour , "%.2d" , t.hr) ;
							sprintf(minute , "%.2d" , t.min) ;
							sprintf(second , "%.2d" , t.sec) ;

							Display_GotoXY(&lcd1 , 50,20);

							Display_Puts(&lcd1 ,hour ,& Font_11x18 , Display_COLOR_WHITE ) ;

							Display_GotoXY(&lcd2 , 50,20);

							Display_Puts(&lcd2 ,minute ,& Font_11x18 , Display_COLOR_WHITE ) ;

							Display_GotoXY(&lcd3 , 50,20);

							Display_Puts(&lcd3 ,second ,& Font_11x18 , Display_COLOR_WHITE ) ;

							Display_UpdateScreen(&lcd1);
							Display_UpdateScreen(&lcd2);
							Display_UpdateScreen(&lcd3);

							state_idle_timer = get_mtick() ;
						}

						if(get_mtick() - sub_state_timer > 3000)
						{
							idel_sub_state =  state_display_date ;
							sub_state_timer = get_mtick() ;
							Display_Clear(&lcd1) ;
							Display_Clear(&lcd2) ;
							Display_Clear(&lcd3) ;
						}

						break ;
					}
					case state_display_date:
					{

						rtc_get_date(&d) ;

						char year[6] ;
						char month[4] ;
						char day[4] ;

						sprintf(year , "20%.2d" , d.year) ;
						sprintf(month , "%.2d" , d.month) ;
						sprintf(day  , "%.2d" , d.day) ;

						Display_GotoXY(&lcd1 , 50,20);

						Display_Puts(&lcd1 ,day ,& Font_11x18 , Display_COLOR_WHITE ) ;

						Display_GotoXY(&lcd2 , 50,20);

						Display_Puts(&lcd2 ,month ,& Font_11x18 , Display_COLOR_WHITE ) ;

						Display_GotoXY(&lcd3 , 40,20);

						Display_Puts(&lcd3 ,year ,& Font_11x18 , Display_COLOR_WHITE ) ;

						Display_UpdateScreen(&lcd1);
						Display_UpdateScreen(&lcd2);
						Display_UpdateScreen(&lcd3);

						if(get_mtick() - sub_state_timer > 3000)
						{
							idel_sub_state =  state_display_time ;
							Display_Clear(&lcd1) ;
							Display_Clear(&lcd2) ;
							Display_Clear(&lcd3) ;
							sub_state_timer = get_mtick() ;
						}

						break ;
					}
					default :
						break ;
				}

				if( key != 0)
				{
					sys_state = state_menu_nav ; //transition
				}

				break ;
			}
			case state_menu_nav:
			{
				switch(menu_nav_sub_state)
				{
					static int menu_index = 0 ;
					static int menu_index_prev  = 0 ;

					case menu_nav_start:
					{
						Display_Clear(&lcd1) ;
						Display_Clear(&lcd2) ;
						Display_Clear(&lcd3) ;
						Display_GotoXY(&lcd1 , 10,20);

						Display_Puts(&lcd1 ,"time" ,& Font_11x18 , Display_COLOR_WHITE ) ;

						Display_GotoXY(&lcd2 , 10,20);

						Display_Puts(&lcd2 ,"date" ,& Font_11x18 , Display_COLOR_WHITE ) ;

						Display_GotoXY(&lcd3 , 10,20);

						Display_Puts(&lcd3 ,"parametre" ,& Font_11x18 , Display_COLOR_WHITE ) ;

						Display_UpdateScreen(&lcd1);
						Display_UpdateScreen(&lcd2);
						Display_UpdateScreen(&lcd3);

						menu_nav_sub_state = menu_nav_update ;
						break ;
					}
					case menu_nav_update:
					{


						if( key == '6' )
						{
							//right
							menu_index_prev = menu_index ;
							menu_index++;
						}
						else if( key == '4')
						{
							//left
							menu_index_prev = menu_index ;
							menu_index--;
						}
						else if( key == '#' )
						{
							//transition
							//confirm
							menu_nav_sub_state = menu_nav_submenu_start ; //go to submenu
							Display_Clear(&lcd1) ;
							Display_Clear(&lcd2) ;
							Display_Clear(&lcd3) ;
							//sys_state =
						}
						else if(key == '*')
						{
							//transiton
							//back
							menu_nav_sub_state = menu_nav_start ; //reset substate
							sys_state = state_idle ;
						}
						else{

						}

						if(menu_index >= 3)
						{
							menu_index = 0 ;
						}

						else if(menu_index < 0)
						{
							menu_index = 2 ;
						}
						else{

						}

						gpio_set_pinState(GPIOA , LED_TAB[menu_index_prev]  , LOW ) ;
						gpio_set_pinState(GPIOA , LED_TAB[menu_index]  , HIGH  ) ;

						//current_lcd

						break ;
					}
					case menu_nav_submenu_start :
					{

						Display_GotoXY(&lcd1 , 10,20);

						Display_Puts(&lcd1 ,(char*)MENU[menu_index] ,& Font_11x18 , Display_COLOR_WHITE ) ;

						for(int i = 0 ; i < submenu_item_lenght[menu_index] ; i++ )
						{
							Display_GotoXY(&lcd2 , 10 , 20 + (9 * i) );
							Display_Puts(&lcd2 ,(char*)SUB_MENU[menu_index][i] ,& Font_7x10 , Display_COLOR_WHITE ) ;
						}

						Display_UpdateScreen(&lcd1);
						Display_UpdateScreen(&lcd2);
						Display_UpdateScreen(&lcd3);

						menu_nav_sub_state = menu_nav_submenu_update ;

						break ;
					}

					case menu_nav_submenu_update :
					{
						static int submenu_index = 0 ;

						if(key == '2')
						{
							Display_Clear(&lcd3) ;
							submenu_index--;
						}
						else if(key == '8')
						{
							Display_Clear(&lcd3) ;
							submenu_index++;
						}

						else if(key == '*')
						{
							//transition
							menu_nav_sub_state = menu_nav_start ; //reset substate
						}


						//test boundry

						if(submenu_index >= submenu_item_lenght[menu_index])
						{
							submenu_index = 0 ;
						}
						else if(submenu_index < 0)
						{
							submenu_index = submenu_item_lenght[menu_index] - 1 ;
						}

						Display_GotoXY(&lcd3 , 10 , 20 );
						Display_Puts(&lcd3 ,(char*)SUB_MENU[menu_index][submenu_index] ,& Font_7x10 , Display_COLOR_WHITE ) ;
						Display_UpdateScreen(&lcd3);

						break ;
					}

					case menu_nav_exit:
					{


						break ;
					}
				}

				break ;
			}
			case state_configuration:
			{
				break ;
			}
			default :
				break ;
		}


		key = keypad_get_pressedkey() ;

		rtc_update() ;

		if(get_mtick() - led_timer > 1000)
		{
			//
			//gpio_toggel(GPIOA , 5) ;
			led_timer = get_mtick() ;
		}


	}
}


/** width 7* height 10
		 * 0x3800, 0x4400, 0x4000, 0x3000, 0x0800, 0x0400, 0x4400, 0x3800, 0x0000, 0x0000,  // S
		 *
		 *   0 0 1 1 1 0 0 0
		 *   0 1 0 0 0 1 0 0
		 *   0 1 0 0 0 0 0 0
		 *   0 0 1 1 0 0 0 0
		 *   0 0 0 0 1 0 0 0
		 *   0 0 0 0 0 1 0 0
		 *   0 1 0 0 0 1 0 0
		 *   0 0 1 1 1 0 0 0
		 *   0 0 0 0 0 0 0 0
		 *   0 0 0 0 0 0 0 0
		 *
		 * 	 0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x5000, 0x2000, 0x0000,  // v
		 *
		 * 	 0 0 0 0 0 0 0 0
		 * 	 0 0 0 0 0 0 0 0
		 * 	 1 0 0 0 1 0 0 0
		 * 	 1 0 0 0 1 0 0 0
		 * 	 1 0 0 0 1 0 0 0
		 *   0 1 0 1 0 0 0 0
		 *   0 0 1 0 0 0 0 0
		 * 	 0 0 0 0 0 0 0 0
		 */


/*
 *
 * This code is setting up a System Timer (SysTick) using the CMSIS library.
 * This The SysTick timer is a 24-bit countdown timer that counts down from the reload value to zero, then wraps back to the reload value.
 * The function systick_enable() is used to enable the SysTick timer and set its reload value.
 * It sets the reload register to the value of (SystemCoreClock_/1000) - 1, where SystemCoreClock_ is the system clock frequency.
 * This will make the timer count down from this value to zero in 1ms intervals.
 * The NVIC_SetPriority function is used to set the priority of the SysTick interrupt.
 * The priority is set to the maximum value, which means that this interrupt has the highest priority among all other interrupts.
 * The SysTick->VAL is loaded with zero and SysTick->CTRL is set to enable the SysTick timer and set its clock source.
 * The SysTick_Handler function is an interrupt service routine that gets called when the SysTick timer reaches zero.
 * This function increments the global variable mtick each time it is called.
 * The delay_ms(uint32_t ms) function is used to delay the program execution for a specific number of milliseconds.
 * It uses the value of the global variable mtick to determine the current time,
 * and it waits in a loop until the value of mtick is greater than or equal to the start time plus the delay time.
 * The get_mtick() function returns the current value of the global variable mtick.
 * Overall, this code is a simple implementation of a delay function that uses the SysTick timer to keep track of time.
 * It is typically used in embedded systems where the processor is expected to perform other tasks while waiting for the delay to complete.
 */
/*
  \brief  Structure type to access the System Timer (SysTick).
 */
void systick_enable()
{    //uint32_t SystemCoreClock_ = 8000000; //8 Mhz
	SysTick->LOAD  = (SystemCoreClock_/1000) - 1;                  /* set reload register */
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Systick Interrupt */
	SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |  /*!< SysTick CTRL: CLKSOURCE Mask */
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

