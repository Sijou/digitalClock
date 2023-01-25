
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
 * TESTED
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
	 * Then the I2C can be enabled by setting the PE bit in the I2C_CR1 register.
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
 * TETSED
 * return the first found address , -1 if no address found
 */
int8_t search_address(I2C_TypeDef * i2c , uint8_t * address)
{
	int cnt = 0 ;
	// 7 bits
	for(int j = 0 ; j < 127 ;j++)
		{

		i2c->CR2 = ((j << 1) << I2C_CR2_SADD_Pos) |
		   (1 << I2C_CR2_NBYTES_Pos);
		 i2c->CR2 |= I2C_CR2_START;

		 uint32_t st = get_mtick() ;

		 while (get_mtick() - st < 20)
		 {
			  if (i2c->ISR & I2C_ISR_NACKF)
			  {
				  i2c->ICR |= (1<<3)|(1<<4); //clear addr and clear start
				 break ;// while(1) ;
			  }
			  if((i2c->ISR & I2C_ISR_TXIS) > 0)
			  {
				  //data = j ;
				  address[cnt] = j ;
				  cnt++ ;
				  //i2c->CR2 &= ~(1<<I2C_CR2_NBYTES_Pos) ;
				  i2c->TXDR = 0x00 ;
				  while ((i2c->ISR & I2C_ISR_TC) == 0) ;

				  i2c->CR2 |= (1<<I2C_CR2_STOP_Pos); //Generate Stop condition
				  delay_ms(100);
				  if(cnt > 3 )
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

void I2C_Write(I2C_TypeDef * i2c , uint8_t address , uint8_t * data , uint8_t len)
{
	i2c->CR2 = ((address << 1) << I2C_CR2_SADD_Pos) | (len << I2C_CR2_NBYTES_Pos) | (0 << I2C_CR2_RD_WRN_Pos | I2C_CR2_AUTOEND);	//send address + data of register + WRITE
		i2c->CR2 |= I2C_CR2_START;
		while ((i2c->ISR & I2C_ISR_TXIS)==0) {	//TXDR is empty
			if ((i2c->ISR & I2C_ISR_NACKF)){
				return;}}

		for(uint8_t j=0; j <= (len-1); j++)
		{
			i2c->TXDR = data[j];
			if(j < (len-1))
			{
				while((i2c->ISR & I2C_ISR_TXIS)==0){}
			}
		}
}





void I2C_Read(I2C_TypeDef * i2c ,uint8_t address , uint8_t * buffer , int len)
{

	i2c->CR2  = ((address << 1) << I2C_CR2_SADD_Pos) ;  // set the slave address
	i2c->CR2 |=   (len << I2C_CR2_NBYTES_Pos);		   // set the number of bytes to be transmitted
	i2c->CR2 |= (1<<I2C_CR2_RD_WRN_Pos);                            	 // i2c read
	i2c->CR2 |= I2C_CR2_START; 					   	// generate the start condition


	 for( int i = 0 ;i<len ;i++)
	 {
		 while((i2c->ISR & I2C_ISR_RXNE) == 0) ; //wait for rx data

		 *buffer = i2c->RXDR ;     //read rx

		 buffer++ ;

		 if(i == len - 1)
		 {
			 while((i2c->ISR & I2C_ISR_TC) == 0) ; //wait for TC Flag
		 }
		 else{
			 //nope
		 }
	 }

	 i2c->CR2 |= (1<<I2C_CR2_STOP_Pos); //Generate Stop condition
}


