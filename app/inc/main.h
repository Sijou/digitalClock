/*
 * main.h
 *
 *  Created on: Dec 24, 2022
 *      Author: Lenovo
 */

#ifndef APP_INC_MAIN_H_
#define APP_INC_MAIN_H_

/*
 * The "system_state" is an enumeration data type that defines different states that a system can be in. It has four different states:
 * state_init: This state is used for displaying a logo when the system is first initialized.
 * state_idle: This state is used for displaying the current time and date when the system is in idle mode.
 * state_menu_nav: This state is used for navigating through the system's menu.
 * state_configuration: This state is used for configuring the system's settings, such as setting alarms, adjusting the time and date, and changing modes.
 */

typedef enum system_state{
	state_init , // display logo
	state_idle , // display time and date
	state_menu_nav ,//menu navigation
	state_configuration //user config alarm , time ,date , modes
}system_state_t ;

/*
 * This defines an enumerated type called "idle_sub_state" which is used to represent the different sub-states of the "state_idle" state.
 * The two enumerated values are "state_display_date" and "state_display_time", indicating that the system can be in one of two sub-states when it is in the "state_idle" state.
 * These sub-states are used to determine what information is being displayed on the screen when the system is in the "state_idle" state.
 */

typedef enum idle_sub_state
{
	state_display_date ,
	state_display_time ,
}idle_sub_state;

/*
 * This enum type is used to define different sub-states for the "state_menu_nav" state of the system.

   "menu_nav_start" is the initial state when the user enters the menu navigation state.
   "menu_nav_update" is the state where the user can update or change options in the menu.
   "menu_nav_exit" is the state where the user exits the menu and returns to the idle state.
 */
typedef enum menu_nav_sub_state{
	menu_nav_start ,
	menu_nav_update ,
	menu_nav_submenu_start ,
	menu_nav_submenu_update ,
	menu_nav_submenu_config ,
	menu_nav_exit ,
}menu_nav_sub_state_t;


typedef enum alarm_state {
	alarm_state_active ,
	alarm_state_desactive ,
	alarm_exit  ,
}alarm_state_t;

void systick_enable() ;
uint32_t get_mtick();
void delay_ms(uint32_t ms);

#endif /* APP_INC_MAIN_H_ */
