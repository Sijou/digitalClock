
/* RTC, "Real-time Clock" (DS3231)
 * Datasheet: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
 *
 * Mit I2C lässt sich dieser RTC ansteuern. Dieser wird vom Master angesteuert und verschiedene Informationen werden vom Master gesendet und ausgelesen.
 * Der RTC besitzt lauf Datasheet die Adresse 11010000 (also 0x68), wobei Bit 1 nicht für die Adresse verwendet wird,
 * sondern um anzugeben, ob ausgelesen oder geschrieben wird (0/1).
 */


#include "stm32f3xx.h"
//#include "rccConfig.h"
#include "i2C.h"
#include "main.h"

/*
 * This function initializes the I2C peripheral on a microcontroller.
 * The function takes a pointer to an I2C_TypeDef struct, which is a register map for the I2C peripheral.
 * The function first checks which I2C peripheral is being initialized by comparing the passed pointer to the pre-defined I2C peripherals (I2C1, I2C2, I2C3).
 * Depending on which peripheral is being initialized, the function sets up the corresponding GPIO pins as alternate function mode and enables the peripheral clock.
 * After that, the function sets up the timing configuration for the I2C peripheral by setting the PRESC, SCLDEL, and SDADEL bits in the TIMINGR register.
 * Finally, the function enables the I2C peripheral by setting the PE bit in the CR1 register.
 */
void I2C_Init(I2C_TypeDef * i2c)
{


 	 if( i2c == I2C1)
 	 {
 		// PB6  I2C1_SCL, PB7   I2C1_SDA
 		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
 		GPIOB->MODER |= (0b10 << GPIO_MODER_MODER6_Pos) |
		   (0b10 << GPIO_MODER_MODER7_Pos); // PB6, PB7: Alternate function
		 GPIOB->AFR[0] |= (4 << GPIO_AFRL_AFRL6_Pos) |
		   (4 << GPIO_AFRL_AFRL7_Pos);         // 0100: AF4

		 RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; //i2c clock enable
 	 }
 	 else if( i2c == I2C2 )
 	 {
 		//PA9 I2C2_SCL , PA10 I2C2_SDA
 		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
 		GPIOA->MODER |= (2 << 18) ;// PA9 10: Alternate function mode
 		GPIOA->MODER |= (2 << 20) ;//PA10 10: Alternate function mode
 		GPIOA->OTYPER |= (1 << 9 ) ;//PA9  GPIO port output type register 1: Output open-drain
 		GPIOA->OTYPER |= (1 << 10) ;//PA10 Oepn Drain
 		GPIOA->OSPEEDR |= (3 << 18) | (3 << 20) ; //PA9,PA10  GPIO port output speed register 11: High speed
 		GPIOA->PUPDR  |= (1<<18) | (1<<20) ;//PA9 ,PA10 GPIO port pull-up/pull-down register 01: Pull-up
 		GPIOA->AFR[1] |= ( 4 << 4 );   //select GPIO alternate function high register  PA9  0100: AF4
 		GPIOA->AFR[1] |= (4 << 8);   //select GPIO alternate function high register  PA10   0100: AF4

 		RCC->APB1ENR |=RCC_APB1ENR_I2C2EN ; //i2c clock enable
 	 }
 	 else if( i2c == I2C3 )
 	 {
 		//PA8 I2C3_SCL , PB5 I2C3_SDA
 		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
 		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

 		GPIOA->MODER   |= (2 << 16) ;// PA8 ALF
 		GPIOB->MODER   |= (2 << 10) ;//PB5 ALF
 		GPIOA->OTYPER  |= (1 << 8 ) ;//PA8 Open Drain
 		GPIOB->OTYPER  |= (1 << 5 ) ;//PB5 Oepn Drain
 		GPIOA->OSPEEDR |= (3 << 16) ;//PA8 High Speed
 		GPIOB->OSPEEDR |= (3 << 10) ; //PB5 High Speed
 		GPIOA->PUPDR   |= (1 << 16) ;//PA8 PULL up
 		GPIOB->PUPDR   |= (1 << 10) ;//PB5 PULL up
 		GPIOA->AFR[1]  |= ( 3 << 0 );//select Alf PA8    0011: AF3
 		GPIOB->AFR[0]  |= (8 << 20) ;//select Alf PB5     1000: AF8

 		RCC->APB1ENR |=RCC_APB1ENR_I2C3EN ; //i2c clock enable
 	 }
	/*
	 * I2C initialization
	 * Enabling and disabling the peripheral
	 * The I2C peripheral clock must be configured and enabled in the clock controller.
	 * Then the I2C can be enabled by setting the PE(PinEnable) bit in the I2C_CR1 register.
	 * When the I2C is disabled (PE=0), the I2C performs a software reset.
	 */
	 i2c->CR1 &= ~I2C_CR1_PE;                // disable the i2c peripheral clock  (clear PE bit in I2C_CR1)
	/*
	 * I2C timings
	 * The timings must be configured in order to guarantee a correct data hold and setup time,
	 * used in master and slave modes. This is done by programming the PRESC,SCLDEL and SDADEL bits in the I2C_TIMINGR register.
	 */
	 i2c->TIMINGR = (1 << I2C_TIMINGR_PRESC_Pos) | // Standard mode 100 kHz @ 8 MHz (page 849/1141)
	             (0x13 << I2C_TIMINGR_SCLL_Pos)  |
	             (0xF << I2C_TIMINGR_SCLH_Pos)   |
	             (0x2 << I2C_TIMINGR_SDADEL_Pos) |
	             (0x4 << I2C_TIMINGR_SCLDEL_Pos);

	 i2c->CR1 |= I2C_CR1_PE;// enable the i2c peripheral clock (set PE bit inI2C_CR1)

}

/**
 * This function is called "search_address" and takes in two parameters: a pointer to an I2C_TypeDef struct, and a pointer to an array of uint8_t.
It returns an int8_t value.

The purpose of this function is to search for devices on the I2C bus by iterating through
all possible 7-bit addresses (0 to 127) and sending a start condition followed by the address.
It then checks if a NACK (non-acknowledgment) or a TXIS (transmit interrupt status) flag is set.
If a NACK flag is set, it means there is no device at that address, so it clears the address and start flags and continues to the next address.
If a TXIS flag is set, it means there is a device at that address,
so it stores that address in the address array, generates a stop condition, and waits for 100ms before continuing to the next address.
It does this for all possible addresses and if cnt (the count of devices found) exceeds 3, it will return -1,
otherwise it will return the count of devices found.
 * TETSED
 * return the first found address , -1 if no address found
 */
int8_t search_address(I2C_TypeDef * i2c , uint8_t * address)
{
	int cnt = 0 ;
	// 7 bits
	for(int j = 0 ; j < 127 ;j++)
		{
        //Sets the slave address, the number of bytes to be transmitted (1), and the start condition in the I2C peripheral's control register 2 (CR2).
		i2c->CR2 = ((j << 1) << I2C_CR2_SADD_Pos) |
		   (1 << I2C_CR2_NBYTES_Pos);
		 i2c->CR2 |= I2C_CR2_START;

		 uint32_t st = get_mtick() ;

		 while (get_mtick() - st < 20)  //Waits for a certain time (20ms) for a response from the slave device.
		 {
			  if (i2c->ISR & I2C_ISR_NACKF)//  If the NACKF (not-acknowledged) flag is set
			  {
				  i2c->ICR |= (1<<3)|(1<<4); // it means that there is no device with that address,
				                            //  so it clears the ADDR and START flags and continues to the next address.
				 break ;// while(1) ;
			  }

			  /* If the TXIS (transmit interrupt status) flag is set
			 			   * it means that the slave device has acknowledged its address,
			 			   * so it stores the address in the provided array,
			 			   * increments the counter variable,
			 			   * sends a data byte (0x00),
			 			   * generates a stop condition,
			 			   * waits for 100ms
			 			   * and repeats the process with the next address.
			 			   */


			  if((i2c->ISR & I2C_ISR_TXIS) > 0)   //If the TXIS (transmit interrupt status) flag is set
			  {

				  //data = j ;
				  address[cnt] = j ;                    //so it stores the address in the provided array,
				  cnt++ ;                               //increments the counter variable,
				  //i2c->CR2 &= ~(1<<I2C_CR2_NBYTES_Pos) ;
				  i2c->TXDR = 0x00 ;                    //sends a data byte (0x00),
				  while ((i2c->ISR & I2C_ISR_TC) == 0) ;

				  i2c->CR2 |= (1<<I2C_CR2_STOP_Pos); //generates a Stop condition
				  delay_ms(100);                     //waits for 100ms
				  if(cnt > 3 )                       //and repeats the process with the next address.
				  {
					  return -1 ;
				  }
				  //return j ;
				  break ;

			  }
			  else{

			  }
		 }

		}
	i2c->CR2 |= (1<<I2C_CR2_STOP_Pos); //Generate Stop condition

	return cnt ;
}

/*
 * This function is used to write data to an I2C device . It takes four parameters as input:

1. I2C_TypeDef *i2c: A pointer to the I2C peripheral that you want to use for communication.
2. uint8_t address: The 7-bit address of the I2C device that you want to communicate with.
3. uint8_t *data: A pointer to the data buffer that you want to send to the I2C device.
4. uint8_t len: The length of the data buffer.

The function first sets the CR2 register by shifting the address left by 1, adding the len and write bit to the register.
Then it sends a start condition using the CR2 register.
After that, it checks if the TXDR register is empty, if not, it checks if the NACK flag is set, if it's set it returns.

After that, it writes the data buffer to the I2C device using a for loop, the loop iterates len-1 times and writes a byte to the TXDR register each time.
 */

void I2C_Write(I2C_TypeDef * i2c , uint8_t address , uint8_t * data , uint8_t len)
{
    //configuring the CR2 register to send the device address, the length of the data buffer, and the write bit.
	i2c->CR2 = ((address << 1) << I2C_CR2_SADD_Pos) | (len << I2C_CR2_NBYTES_Pos)
			                                        | (0 << I2C_CR2_RD_WRN_Pos | I2C_CR2_AUTOEND);
    //sending start condition
		i2c->CR2 |= I2C_CR2_START;
    //waiting until the TXDR register is empty
		while ((i2c->ISR & I2C_ISR_TXIS)==0) {
			if ((i2c->ISR & I2C_ISR_NACKF)){
				return;}}
    //writing data buffer to the I2C device using a for loop
		for(uint8_t j=0; j <= (len-1); j++)
		{
			i2c->TXDR = data[j];
			if(j < (len-1))
			{
                //waiting until the TXIS flag is set
				while((i2c->ISR & I2C_ISR_TXIS)==0){}
			}
		}
}


/*
 * The I2C_Read function is used to read data from an I2C device using the I2C peripheral of the STM32F303RE microcontroller.

First, the function sets the slave address of the I2C device and the number of bytes to be read in the CR2 register.
Then it sets the read bit in the CR2 register to indicate that the operation is a read operation.
After that, it generates a start condition by setting the START bit in the CR2 register.

Then, the function enters a for loop to read the data from the I2C device.
In each iteration of the loop, it waits for the RXNE flag to be set, indicating that there is data in the RXDR register.
Once the flag is set, it reads the data from the RXDR register and stores it in the buffer.
It then increments the buffer pointer to move to the next memory location.

If this is the last iteration, it waits for the TC flag to be set indicating the end of the transmission.
If not, the loop continues until all the data is read.

Finally, the function generates a stop condition by setting the STOP bit in the CR2 register.
This stops the current I2C communication and releases the bus for other devices to use it.
 */


void I2C_Read(I2C_TypeDef * i2c ,uint8_t address , uint8_t * buffer , int len)
{
    //setting the slave address and the number of bytes to be transmitted
    i2c->CR2  = ((address << 1) << I2C_CR2_SADD_Pos) ;
	i2c->CR2 |=   (len << I2C_CR2_NBYTES_Pos);
    //setting the read bit
	i2c->CR2 |= (1<<I2C_CR2_RD_WRN_Pos);
    //generating start condition
	i2c->CR2 |= I2C_CR2_START;

    //reading data from the i2c device using a for loop
	 for( int i = 0 ;i<len ;i++)
	 {
        //waiting for the RXNE flag to be set
		 while((i2c->ISR & I2C_ISR_RXNE) == 0) ;
        //reading the data from the RXDR register
		 *buffer = i2c->RXDR ;
        //incrementing the buffer pointer
		 buffer++ ;

		 if(i == len - 1)
		 {
            //waiting for the TC flag to be set
			 while((i2c->ISR & I2C_ISR_TC) == 0) ;
		 }
		 else{
			 //nope
		 }
	 }
}

    //generating stop condition


