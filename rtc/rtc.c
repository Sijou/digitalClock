#include "stm32f303xe.h"
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "stdbool.h"
#include "stdio.h"

I2C_TypeDef * rtc_i2c ;
uint8_t tm;

/*
 * This function writes data to a specified register of an RTC (Real-time clock) device connected through an I2C bus.
 * The function takes in four parameters:
 * - The address of the RTC device on the I2C bus (a uint8_t)
 * - The register to write the data to (a uint8_t)
 * - The data to be written (a uint8_t)
 * - the lenght of the data to be written (an int)
 */
static void rtc_write_reg(uint8_t address ,uint8_t reg , uint8_t  data , int len)
{
    // Declare a temporary array of 2 elements
    uint8_t tmp[2] ;

    // Assign the value of the register to the first element of the temporary array
    tmp[0]  = reg ;
    // Assign the value of the data to the second element of the temporary array
    tmp[1]  = data ;

    // Call the I2C_Write function to send the data in the temporary array to the RTC device at the specified address on the I2C bus.
    I2C_Write(rtc_i2c , address , tmp ,2 ) ;
}

/*
 * This function initializes the RTC (Real-time clock) device.
 * The function takes in one parameter:
 * - The I2C peripheral of the device (I2C_TypeDef)
 * The function sets the global variable rtc_i2c to the I2C peripheral passed as a parameter.
 * This allows the other functions of the RTC driver to use the same I2C peripheral without having to pass it as a parameter every time.
 * It assigns the passed I2C peripheral to a global variable so that the same peripheral can be used by other functions
 *  without having to pass it as a parameter every time.
 */
void rtc_init(I2C_TypeDef * dev)
{
    // Assign the passed I2C peripheral to a global variable
    rtc_i2c = dev ;
}


/*
 * This function retrieves the value of a specified register from the RTC (Real-time clock) device.
 * The function takes in one parameter:
 * - The address of the register (uint8_t)
 */
uint8_t rtc_get_reg(uint8_t reg)
{
    // Declare a variable to hold the value of the register
    uint8_t val ;

    // Send the register address to the RTC device
    I2C_Write(rtc_i2c , RTC_I2C_ADDR , &reg  , 1) ;

    // Wait for 2ms
    delay_ms(2);

    // Read the value of the register from the RTC device
    I2C_Read(rtc_i2c  , RTC_I2C_ADDR , &val , 1) ;

    // Return the value of the register
    return val ;
}

/*
 * This function is used to get the current time from the RTC device and store it in a rtc_time_t struct.
 * It takes a pointer to the rtc_time_t struct as an argument and uses it to store the time values.
 * The function first reads the value of the seconds register using the rtc_get_reg() function and stores it in a temporary variable.
 * Then it converts the BCD value of the seconds to decimal and stores it in the "sec" field of the rtc_time_t struct.
 * It then proceeds to do the same for the minutes and hours registers, converting the BCD values to decimal and storing them in the "min" and "hr" fields respectively.
 * The function then checks the mode of the RTC device (12 hour or 24 hour) using the RTC_BIT_MODE_MASK.
 * If it is in 12 hour mode, it also checks the AM/PM bit using the RTC_AM_PM_BIT_MASK and stores the value in the "am_pm" field of the struct.
 * After all values are stored, the function returns the struct.
*/

void rtc_get_time(rtc_time_t * rtc_time )
{
	uint8_t tmp = rtc_get_reg(RTC_REG_SECOND) ;
	tm = tmp ;
	//BCD --> DEC
	rtc_time->sec  = ((tmp >> 4) & 0x0f) * 10 + (tmp & 0x0f) ;

	tmp = rtc_get_reg(RTC_REG_MINUTE) ;
	//BCD --> DEC
	rtc_time->min  = ((tmp >> 4) & 0x0f) * 10 + (tmp & 0x0f) ;

	tmp = rtc_get_reg(RTC_REG_HOUR) ;


	if( (tmp & RTC_BIT_MODE_MASK) != 0)
	{
		//12 hour mode
		rtc_time->mode = H_12 ;
		if((tmp & RTC_AM_PM_BIT_MASK) != 0)
		{
			rtc_time->am_pm = PM ;
		}
		else{
			rtc_time->am_pm = AM ;
		}

		rtc_time->hr = ((tmp & 0x1f) >> 4) * 10 + (tmp & 0x0f) ;
	}
	else
	{
		//24 hour mode
		rtc_time->mode = H_24 ;
		tmp = RTC_24H_MASK & tmp ;
		rtc_time->hr = (tmp >> 4) * 10 + (tmp & 0x0f) ;
	}

}


/*
 * The code sets the time on an RTC (real-time clock) device using the I2C protocol.
 * It takes a pointer to an "rtc_time_t" struct which contains the current time (hours, minutes, and seconds).
 * The code first gets the tens and ones digits of the seconds, minutes and hours by using integer division and modulo operations.
 * It then concatenates the tens digit and ones digit by shifting the tens digit left by 4 bits and ORing it with the ones digit.
 * This creates a single 8-bit value that represents the time in BCD (binary-coded decimal) format.
 * Finally, the code writes this value to the appropriate register on the RTC device
 * using the "rtc_write_reg" function with the RTC's I2C address, register address, and data value passed as arguments,
 * as well as a flag indicating whether or not to wait for the write to complete.
 * The code uses three variables high, low, and tmp to hold the tens and ones digits of the seconds, minutes, and hours and the BCD format respectively
 * It then uses these variables to set the time on RTC device by writing the BCD format value to the appropriate register on the RTC device
 * The function rtc_write_reg is used to write the value to the RTC device,
 * it takes 4 arguments I2C address of RTC, register address of RTC, data value to be written and a flag indicating whether to wait for the write to complete or not.
 * This function is useful for setting the time on an RTC device, which uses I2C protocol. The BCD format is used to represent the time in a more compact form.
*/
void rtc_set_time(rtc_time_t * rtc_time )
{
    // Separate the tens and ones digits of the seconds
    uint8_t high = rtc_time->sec / 10 ;
    uint8_t low  = rtc_time->sec % 10 ;

    // Concatenate the tens and ones digits to create a BCD value
    uint8_t tmp = (high << 4) | low ;

    // Write the BCD value to the seconds register on the RTC device
    rtc_write_reg( RTC_I2C_ADDR ,RTC_REG_SECOND ,tmp , 1) ;

    // Repeat the process for the minutes
    high = rtc_time->min / 10 ;
    low  = rtc_time->min % 10 ;
    tmp = (high << 4) | low ;
    rtc_write_reg( RTC_I2C_ADDR ,RTC_REG_MINUTE ,tmp , 1) ;

    // Repeat the process for the hours
    high = rtc_time->hr / 10 ;
	low  = rtc_time->hr % 10 ;

	tmp = (high << 4) | low ;

	rtc_write_reg( RTC_I2C_ADDR ,RTC_REG_HOUR ,tmp , 1) ;

}
/*
 * The code is a function called "rtc_set_date" that takes a pointer to an "rtc_date_t" struct, which contains the current date (day, month, and year).
 * The function starts by initializing a variable "high" with the result of the integer division of the day by 10, which gives the tens digit of the day. It then initializes a variable "low" with the result of the modulo operation of the day by 10, which gives the ones digit of the day.
 * It then concatenates the tens and ones digits to create a BCD value and assigns it to a variable "tmp".
 * The function then uses the "rtc_write_reg" function to write the BCD value of the day to the date register on the RTC device.
 * The function then repeats the process for the month and year, getting the tens and ones digits, concatenating them to create a BCD value,
 * and writing the BCD value to the month and year registers on the RTC device.
 * his function is useful for setting the date on an RTC device, which uses I2C protocol. The BCD format is used to represent the date in a more compact form.
 */
void rtc_set_date(rtc_date_t *date)
{
    // Separate the tens and ones digits of the day
    uint8_t high = date->day / 10 ;
    uint8_t low  = date->day % 10 ;

    // Concatenate the tens and ones digits to create a BCD value
    uint8_t tmp  = (high << 4 )|low ;

    // Write the BCD value to the day register on the RTC device
    rtc_write_reg( RTC_I2C_ADDR , RTC_REG_DATE ,tmp , 1) ;

    // Repeat the process for the month
    high = date->month / 10 ;
    low  = date->month % 10 ;
    tmp  = (high << 4 )|low ;
    rtc_write_reg( RTC_I2C_ADDR , RTC_REG_MONTH ,tmp , 1) ;

    // Repeat the process for the year
    high = date->year / 10 ;
    low  = date->year % 10 ;
    tmp  = (high << 4 )|low ;
    rtc_write_reg( RTC_I2C_ADDR , RTC_REG_YEAR ,tmp , 1) ;
}
/*
 * This function is responsible for getting the date from an RTC device via I2C communication. It takes a pointer to an "rtc_date_t" struct,
 * which will be filled with the current date (day, month, and year).
 * The function starts by reading the date register from the RTC device using the "rtc_get_reg" function.
 * It then extracts the tens and ones digits of the day from the BCD value using bitwise operations.
 * The function then reads the month register from the RTC device and clears the century bit by ANDing it with 0x7f.
 * It then extracts the tens and ones digits of the month from the BCD value in the same way as the day.
 * Finally, the function reads the year register from the RTC device and extracts the tens and ones digits of the year from the BCD value using bitwise operations.
 */
void rtc_get_date(rtc_date_t * date)
{
    // Read the date register from the RTC device
    uint8_t tmp = rtc_get_reg(RTC_REG_DATE) ;

    // Extract the tens and ones digits from the BCD value
    date->day = (tmp >> 4)*10 + (tmp & 0x0f) ;

    // Read the month register from the RTC device
    tmp = rtc_get_reg(RTC_REG_MONTH) ;

    // Clear the century bit
    tmp &= 0x7f ;

    // Extract the tens and ones digits from the BCD value
    date->month = (tmp >> 4) *10 +(tmp & 0x0f) ;

    // Read the year register from the RTC device
    tmp = rtc_get_reg(RTC_REG_YEAR) ;

    // Extract the tens and ones digits from the BCD value
    date->year = (tmp >> 4) *10 +(tmp & 0x0f) ;
}




/*
 * This function is responsible for setting an alarm on an RTC device via I2C communication. It takes a pointer to an "rtc_time_t" struct,
 * which contains the alarm time (seconds, minutes, and hours).
 * The function starts by using the "GET_BCD_HIGH" and "GET_BCD_LOW" macros to convert the alarm time to BCD format.
 * It then writes the BCD-formatted alarm time to the appropriate registers (seconds, minutes, and hours) on the RTC device using the "rtc_write_reg" function.
 * Next, the function sets the A1M4 bit in the alarm date register to 1, indicating that the alarm should trigger when the seconds, minutes, and hours match.
 * The function then clears the alarm flag by ANDing the status register with 0xfe, and enables the alarm by setting the AE1 bit in the control register.
*/


void rtc_set_alarm(rtc_time_t * alarm)
{
// Convert the alarm time to BCD format
uint8_t tmp = GET_BCD_HIGH(alarm->sec) | GET_BCD_LOW(alarm->sec);

// Write the BCD-formatted alarm time to the seconds register
rtc_write_reg( RTC_I2C_ADDR , RTC_A1_SECOND ,tmp , 1) ;

	tmp = GET_BCD_HIGH(alarm->min)  | GET_BCD_LOW(alarm->min)   ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_MINUTE ,tmp , 1) ;

	tmp = GET_BCD_HIGH(alarm->hr)  | GET_BCD_LOW(alarm->hr)     ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_HOUR ,tmp , 1) ;

	//set alarm when sec ,mi, and hour matchs

	tmp = rtc_get_reg(RTC_A1_DATE) ;

	//set A1M4 bit to one
	tmp |= 0x80 ;

	rtc_write_reg( RTC_I2C_ADDR , RTC_A1_DATE ,tmp , 1) ;

	//clear alarm flag
	tmp  = rtc_get_reg(RTC_STATUS_REG) & 0xfe; // set A1F to 0

	rtc_write_reg( RTC_I2C_ADDR , RTC_STATUS_REG ,tmp , 1) ;

	//enable rtc alarm
	tmp = rtc_get_reg(RTC_CONTROL_REG) ;

	//set AE1 in control reg
	tmp |= 0x01;

	rtc_write_reg( RTC_I2C_ADDR , RTC_CONTROL_REG ,tmp , 1) ;

}

/*
 * This function is responsible for getting the alarm time from an RTC device via I2C communication. It takes a pointer to an "rtc_time_t" struct,
 * which will be filled with the alarm time (seconds, minutes, and hours).
 * The function starts by reading the alarm seconds register from the RTC device using the "rtc_get_reg" function.
 * It then extracts the tens and ones digits of the seconds from the BCD value using bitwise operations.
 * The function then reads the alarm minutes register from the RTC device and extracts the tens and ones digits of the minutes from the BCD value in the same way as the seconds.
 * Finally, the function reads the alarm hours register from the RTC device and extracts the tens and ones digits of the hours from the BCD value using bitwise operations.
*/
void rtc_get_alarm(rtc_time_t * tm)
{
// Read the alarm seconds register from the RTC device
uint8_t tmp = rtc_get_reg(RTC_A1_SECOND) ;

// Extract the tens and ones digits from the BCD value
tm->sec = (tmp >> 4) *10 + (tmp & 0x0f) ;

// Read the alarm minutes register from the RTC device
tmp = rtc_get_reg(RTC_A1_MINUTE) ;

// Extract the tens and ones digits from the BCD value
tm->min = (tmp >> 4) *10 + (tmp & 0x0f) ;

// Read the alarm hours register from the RTC device
tmp = rtc_get_reg(RTC_A1_HOUR) ;

// Extract the tens and ones digits from the BCD value
tm->hr = (tmp >> 4) *10 + (tmp & 0x0f) ;
}

/*
 * This function is used to check if the alarm is set on the RTC device.
 * It first reads the status register using the "rtc_get_reg" function.
 * Then it checks if the A1F bit is set in the status register by ANDing it with the RTC_STATUS_AF1_MASK.
 * If the A1F bit is set, it returns true, indicating that the alarm is set.
 * Otherwise, it returns false, indicating that the alarm is not set.
*/
bool rtc_is_alarm_set( )
{
bool ret = false ;
// Check if A1F in status register is set
if( (rtc_get_reg(RTC_STATUS_REG) & RTC_STATUS_AF1_MASK ) != 0 )
{
ret = true ;
}
else{
ret = false ;
}
return ret ;
}

/*
 * This function is responsible for clearing the alarm flag in the RTC device.
 * It first reads the status register using the "rtc_get_reg" function.
 * Then it uses bitwise operations to clear the A1F bit in the register.
 * Finally, it writes the updated value back to the status register using the "rtc_write_reg" function.
*/
void rtc_alarm_clear()
{
// Read the status register
uint8_t tmp = rtc_get_reg(RTC_STATUS_REG) ;

// Clear the A1F bit
tmp &= ~RTC_STATUS_AF1_MASK ;

// Write the updated value back to the status register
rtc_write_reg( RTC_I2C_ADDR , RTC_STATUS_REG ,tmp , 1) ;
}
/*
 * This function is responsible for setting the callback function that will be called when the alarm goes off.
 * The function takes a single argument,which is a pointer to a callback function of the form callback_t.
 * This callback function will be called when the alarm goes off and can be used to perform any necessary actions such as turning on a light, sending an alert, etc.
 * The function stores the passed callback function in a global variable named alarm1_callback,
 * so that it can be called later when the alarm goes off.
 */
callback_t 	alarm1_callback  ;

void rtc_set_alarm_callback(callback_t  callback)
{
	alarm1_callback = callback ;
}

/*
 * The rtc_update() function is used to check if an alarm has been set by the RTC device and if so,
 * it calls the callback function that was previously set using rtc_set_alarm_callback().
 * If the alarm is not set, the function does nothing.
 * This function should be called periodically in the main loop of the program to check for the alarm status and respond accordingly.
 */
void rtc_update(void)
{
	if(rtc_is_alarm_set( ) == true && alarm1_callback != NULL)
	{
		alarm1_callback() ;//NULL
	}
	else
	{

	}
}

