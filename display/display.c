//ok
#include "stm32f3xx.h"
#include "display.h"
#include "fonts.h"
#include "i2c.h"

// Write Command/ Data/ Buffer
#define Display_WriteCommand(oled ,command)      Display_I2C_Write(oled , 0x3C, 0x00, (command), 2)
#define Display_WriteData(oled ,data)            Dispaly_I2C_Write(oled ,0x3C, 0x40, (data), 2)
//#define SSD1306_WRITECOMMAND(command)      SSD_I2C_Write(0x3C, 0x00, (command), 2)
//#define SSD1306_WRITEDATA(data)            SSD_I2C_Write(0x3C, 0x40, (data), 2)


      // 8bit             *       128* 64 /8
//static uint8_t Display_Buffer[Display_WIDTH * Display_HEIGHT / 8];

// Absolute value (filled Triangle)
#define ABS(x)   ((x) > 0 ? (x) : -(x))



//static SSD1306_t Display;

#define Display_RIGHT_HORIZONTAL_SCROLL              0x26
#define Display_LEFT_HORIZONTAL_SCROLL               0x27
#define Display_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define Display_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define Display_DEACTIVATE_SCROLL                    0x2E // Stop scroll
#define Display_ACTIVATE_SCROLL                      0x2F // Start scroll
#define Display_SET_VERTICAL_SCROLL_AREA             0xA3 // Set scroll range

#define Display_NORMALDISPLAY       0xA6
#define Display_INVERTDISPLAY       0xA7



/*
 * This function is used for displaying a bitmap image on an OLED display.
 * It takes in a pointer to an OLED object, as well as the x and y coordinates of where the image should be displayed on the screen,
 * a pointer to the bitmap image data, the width and height of the image, and the color of the image.
 * The function uses a nested loop to iterate through the image data and draw each pixel on the OLED display using the Display_DrawPixel function.
 * The function also calculates the byte width of the image and uses bit shifting to extract the appropriate pixel data from the bitmap image data.
 */

void Display_DrawBitmap(OLED_t *oled ,uint16_t x, uint16_t y, const unsigned char *bitmap, uint16_t w, uint16_t h, Display_COLOR_t color)
{

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++)
    {
        for(int16_t i=0; i<w; i++)
        {
            if(i & 7)
            {
               byte <<= 1;
            }
            else
            {
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
            if(byte & 0x80)
            	Display_DrawPixel(oled ,x+i, y, color);
        }
    }
}

/*
 * This function initializes an OLED display by sending a series of commands to configure its memory addressing, contrast, and other settings.
 * It also sets the OLED's internal "CurrentX" and "CurrentY" variables to 0, and sets the "Initialized" variable to 1 to indicate that the OLED has been initialized successfully.
 * The function also deactivates scrolling on the OLED and fills the screen with black color.
 * At the end, the function returns 1 to indicate that the initialization was successful.
 */


uint8_t Display_Init(OLED_t * oled) {

	//delay
	uint32_t p = 2500;
	while(p>0)
		p--;

	Display_WriteCommand(oled , 0xAE); //display off
	Display_WriteCommand(oled , 0x20); //Set Memory Addressing Mode
	Display_WriteCommand(oled , 0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	Display_WriteCommand(oled , 0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	Display_WriteCommand(oled , 0xC8); //Set COM Output Scan Direction
	Display_WriteCommand(oled , 0x00); //---set low column address
	Display_WriteCommand(oled , 0x10); //---set high column address
	Display_WriteCommand(oled , 0x40); //--set start line address
	Display_WriteCommand(oled , 0x81); //--set contrast control register
	Display_WriteCommand(oled , 0xFF);
	Display_WriteCommand(oled , 0xA1); //--set segment re-map 0 to 127
	Display_WriteCommand(oled , 0xA6); //--set normal display
	Display_WriteCommand(oled , 0xA8); //--set multiplex ratio(1 to 64)
	Display_WriteCommand(oled , 0x3F); //
	Display_WriteCommand(oled , 0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Display_WriteCommand(oled , 0xD3); //-set display offset
	Display_WriteCommand(oled , 0x00); //-not offset
	Display_WriteCommand(oled , 0xD5); //--set display clock divide ratio/oscillator frequency
	Display_WriteCommand(oled , 0xF0); //--set divide ratio
	Display_WriteCommand(oled , 0xD9); //--set pre-charge period
	Display_WriteCommand(oled , 0x22); //
	Display_WriteCommand(oled , 0xDA); //--set Com pins hardware configuration
	Display_WriteCommand(oled , 0x12);
	Display_WriteCommand(oled , 0xDB); //--set vcomh
	Display_WriteCommand(oled , 0x20); //0x20,0.77xVcc
	Display_WriteCommand(oled , 0x8D); //--set DC-DC enable
	Display_WriteCommand(oled , 0x14); //
	Display_WriteCommand(oled , 0xAF); //--turn on SSD1306 panel


	Display_WriteCommand(oled ,Display_DEACTIVATE_SCROLL);
	//Display_Fill(Display_COLOR_WHITE);	// Clear screen
	Display_Fill(oled ,Display_COLOR_BLACK);	// Clear screen
	Display_UpdateScreen(oled);		// Update screen

	// Set default values
	oled->CurrentX = 0;
	oled->CurrentY = 0;

	// Initialized OK
	oled->Initialized = 1;

	// Return OK
	return 1;
}
/*
 * This function updates the screen of the OLED display by writing the contents of the buffer "oled->Display_Buffer" to the screen.
 * It does this by iterating through each of the 8 pages (m) on the OLED display and setting the page address to the current page (0xB0 + m).
 * It then sets the column address to the start of the page (0x00 and 0x10) and writes the contents
 * of the corresponding section of the buffer (Display_WIDTH * m) to the screen using the Display_I2C_WriteMulti function.
 */
void Display_UpdateScreen(OLED_t * oled) {
	uint8_t m;

	for (m = 0; m < 8; m++) {
		Display_WriteCommand(oled , 0xB0 + m);
		Display_WriteCommand(oled , 0x00);
		Display_WriteCommand(oled , 0x10);

		Display_I2C_WriteMulti(oled , 0x3C, 0x40, &(oled->Display_Buffer[Display_WIDTH * m]), Display_WIDTH);

	}
}

/*
 * The function Display_Fill is used to fill the OLED display's buffer (oled->Display_Buffer) with a specified color.
 * The function takes in two arguments: a pointer to an OLED structure (oled) and the color to fill the display with (color).
 * The memset function is then used to fill the buffer with either 0x00 (black) or 0xFF (white) depending on the color passed in as an argument.
 * The number of bytes to be filled is equal to the size of the buffer, which is defined by sizeof(oled->Display_Buffer).
 * This function is used to clear the screen and set all pixels to the specified color.
 */
void Display_Fill(OLED_t * oled , Display_COLOR_t color) {
	// Set memory, fill a block of memory with value
	memset(oled->Display_Buffer, (color == Display_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(oled->Display_Buffer));
  //memset(starting address, value to be filled, number of bytes to be filled)
}


/*
 * This function is for drawing a single pixel on the OLED display.
 * It takes in a pointer to an OLED structure, x and y coordinates for the pixel, and a color for the pixel.
 * It first checks if the coordinates given are within the bounds of the display's width and height.
 * If they are not, the function exits. Next, it checks if the display is set to be inverted and, if it is, it inverts the color of the pixel.
 * Finally, it sets the color of the pixel in the Display_Buffer using bit manipulation. If the color is white, it sets the corresponding bit in the buffer to 1.
 * If the color is black, it sets the corresponding bit to 0.
 * This allows the display to render the pixel on the screen.
 */
void Display_DrawPixel(OLED_t * oled ,uint16_t x, uint16_t y, Display_COLOR_t color) {

	if (x >= Display_WIDTH || y >= Display_HEIGHT)
	{
		// Error
		return;
	}

	// Check if pixels are inverted
	if (oled->Inverted) {
		color = (Display_COLOR_t)!color;
	}

	// Set color
	if (color == Display_COLOR_WHITE) {
		oled->Display_Buffer[x + (y / 8) * Display_WIDTH] |= 1 << (y % 8);
	} else {
		oled->Display_Buffer[x + (y / 8) * Display_WIDTH] &= ~(1 << (y % 8));
	}
}

/*
 * This function sets the current write position on the OLED display to the specified x and y coordinates.
 * The OLED_t struct pointer "oled" is passed in as a parameter, and the struct contains the current x and y position as members "CurrentX" and "CurrentY", respectively.
 * The function updates these members with the new x and y coordinates passed in as parameters.
 * This allows for easy control over the position for subsequent writes or drawing operations.
 */
void Display_GotoXY(OLED_t * oled ,uint16_t x, uint16_t y) {
	// Set write pointers
	oled->CurrentX = x;
	oled->CurrentY = y;
}

/*
 * This function takes in a pointer to an OLED structure, a character to be written, a pointer to a font definition, and a color.
 * It checks if there is enough space on the OLED display to write the character in the current position,
 * and if there is, it uses the font definition to determine which pixels should be turned on or off to write the character.
 * It then increments the current X position on the OLED structure so that the next character can be written in the correct position.
 * The function returns the character that was written.
 */
char Display_Putc(OLED_t * oled ,char ch, FontDef_t *Font, Display_COLOR_t color) {
	uint32_t i, b, j;

	// Check available space in LCD
	if (
		Display_WIDTH <= (oled->CurrentX + Font->FontWidth) ||
		Display_HEIGHT <= (oled->CurrentY + Font->FontHeight)
	) {
		// Error
		return 0;
	}

	// Go through font
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				Display_DrawPixel(oled ,oled->CurrentX + j, (oled->CurrentY + i), (Display_COLOR_t) color);
			} else {
				Display_DrawPixel(oled ,oled->CurrentX + j, (oled->CurrentY + i), (Display_COLOR_t)!color);
			}
		}
	}

	// Increase pointer
	oled->CurrentX += Font->FontWidth;

	// Return character written
	return ch;
}

/*
 * This function takes in a pointer to an OLED structure, a string, a pointer to a font definition, and a color as inputs.
 * It then writes the characters of the string to the OLED display, one by one, using the Display_Putc function to write each individual character.
 * The function starts by setting a pointer to the first character of the string and then loops through each character of the string. For each character,
 * the function calls the Display_Putc function to write the character to the OLED display and then increments the pointer to the next character in the string.
 * If there is an error while writing a character, the function returns that character.
 * If all the characters in the string are successfully written to the OLED display, the function returns the last character of the string (which should be a null character).
 */
char Display_Puts(OLED_t * oled ,char* str, FontDef_t* Font, Display_COLOR_t color) {
	// Write characters
	while (*str) {
		// Write character by character
		if (Display_Putc(oled ,*str, Font, color) != *str) {
			// Return error
			return *str;
		}

		// Increase string pointer
		str++;
	}

	// Everything OK, zero should be returned
	return *str;
}


/*
 * The function Display_Clear() is used to clear the contents of the OLED screen.
 * It first calls the Display_Fill() function to fill the entire display buffer with black (0) color.
 * Then it calls Display_UpdateScreen() to update the OLED screen with the new display buffer.
 * This effectively clears the entire OLED screen and sets it to a blank black display.
 */

void Display_Clear (OLED_t * oled )
{
	Display_Fill (oled , 0);
	Display_UpdateScreen(oled);
}
/*
 * The function Display_ON() is used to turn on the OLED display.
 * It does so by sending three commands to the OLED display.
 * The first command is 0x8D, which sets the charge pump voltage to enable the OLED display.
 * The second command is 0x14, which sets the charge pump to enable the OLED display.
 * The third command is 0xAF, which sets the OLED display to turn on.
 */
void Display_ON(OLED_t * oled ) {
	Display_WriteCommand(oled , 0x8D);
	Display_WriteCommand(oled , 0x14);
	Display_WriteCommand(oled , 0xAF);
}
/*
 * this function  turns off the OLED by sending command 0x8D, 0x10 and 0xAE which will set the charge pump off and the display off.
 */
void Display_OFF(OLED_t * oled ) {
	Display_WriteCommand(oled , 0x8D);
	Display_WriteCommand(oled , 0x10);
	Display_WriteCommand(oled , 0xAE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  _____ ___   _____
// |_   _|__ \ / ____|
//   | |    ) | |
//   | |   / /| |
//  _| |_ / /_| |____
// |_____|____|\_____|
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * This function is used to write multiple bytes of data to an I2C device at a specific address, using the specified data register.
 * The OLED_t struct pointer is passed as the first argument, followed by the I2C device address, the data register address,
 * a pointer to the data to be written, and the number of bytes to be written.
 * The function uses the CR2 register of the I2C peripheral to configure the address, number of bytes, and read/write direction.
 * It then sends a start condition and waits for the TXIS (transmit interrupt status) flag to be set before sending the data register address and the data to be written.
 * The data is sent one byte at a time, and the function waits for the TXIS flag to be set before sending the next byte. Once all the data has been sent, the function ends.
 */


void Display_I2C_WriteMulti(OLED_t * oled,uint8_t address, uint8_t dataRegister, uint8_t *data , uint16_t numBytes)	//data is a pointer
{
	uint8_t i;
	uint8_t j;
	uint8_t dt[numBytes];		//necessary for multi Write
	I2C_TypeDef *  i2c = oled->dev;
	i2c->CR2 = ((address << 1) << I2C_CR2_SADD_Pos) | (numBytes << I2C_CR2_NBYTES_Pos) | (0 << I2C_CR2_RD_WRN_Pos | I2C_CR2_AUTOEND);	//send address + data of register + WRITE
	i2c->CR2 |= I2C_CR2_START;
	while ((i2c->ISR & I2C_ISR_TXIS)==0) {	//TXDR is empty
		if ((i2c->ISR & I2C_ISR_NACKF)){
			return;}}

	dt[0] = dataRegister;
	for(i=0; i<numBytes; i++)
	{
	dt[i+1] = data[i];	//necessary for multi Write
	}

	for(j=0; j <= (numBytes-1); j++)
	{
		i2c->TXDR = dt[j];
		if(j < (numBytes-1))
		{
			while((i2c->ISR & I2C_ISR_TXIS)==0){}		//ggf TXIS mit TXE ergänzen
		}
	}

}

/*
 * This function writes a single byte of data to a specified I2C address and register using the I2C peripheral specified in the OLED_t struct.
 * The address, data register, and data to be written are passed as arguments to the function.
 * The number of bytes to be written (in this case, 2 bytes, one for the register and one for the data) is also passed as an argument.
 * The function starts the I2C communication and waits for the transmit buffer to be empty before sending the register and data.
 * It then waits for the transmit buffer to be empty again before sending the data. The function will exit and return if a NACK is received during the transmission.
 */
void Display_I2C_Write(OLED_t * oled, uint8_t address, uint8_t dataRegister, uint8_t data , uint8_t numBytes)
{


	uint8_t j;
	uint8_t dt[2];
	I2C_TypeDef *  i2c = oled->dev;
	dt[0] = dataRegister;
	dt[1] = data;

	i2c->CR2 = ((address << 1) << I2C_CR2_SADD_Pos) | (numBytes << I2C_CR2_NBYTES_Pos) | (0 << I2C_CR2_RD_WRN_Pos | I2C_CR2_AUTOEND);	//send address + data of register + WRITE
	i2c->CR2 |= I2C_CR2_START;
	while ((i2c->ISR & I2C_ISR_TXIS)==0) {	//TXDR is empty
		if ((i2c->ISR & I2C_ISR_NACKF)){
			return;}}

	for(j=0; j <= (numBytes-1); j++)
	{
		i2c->TXDR = dt[j];
		if(j < (numBytes-1))
		{
			while((i2c->ISR & I2C_ISR_TXIS)==0){}
		}
	}

}


void Display_printTime(OLED_t *oled ,uint8_t *rtc_bufferCalculated, FontDef_t *Font11x18)
{
	//seconds
	Display_GotoXY(oled ,0, 0);
	Display_Putc(oled ,(rtc_bufferCalculated[5]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,11, 0);
	Display_Putc(oled ,(rtc_bufferCalculated[4]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,22, 0);
	Display_Putc(oled ,':' , &Font_11x18, 1);
	//minutes
	Display_GotoXY(oled ,33, 0);
	Display_Putc(oled ,(rtc_bufferCalculated[3]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,44, 0);
	Display_Putc(oled ,(rtc_bufferCalculated[2]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,55, 0);
	Display_Putc(oled ,':' , &Font_11x18, 1);
	//hours
	Display_GotoXY(oled ,66, 0);
	Display_Putc(oled ,(rtc_bufferCalculated[1]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,77, 0);
	Display_Putc(oled ,(rtc_bufferCalculated[0]+48) , &Font_11x18, 1);
	//date
	Display_GotoXY(oled ,0, 18);
	Display_Putc(oled ,(rtc_bufferCalculated[8]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,11, 18);
	Display_Putc(oled ,(rtc_bufferCalculated[7]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,22, 18);
	Display_Putc(oled ,'.' , &Font_11x18, 1);
	//month
	Display_GotoXY(oled ,33, 18);
	Display_Putc(oled ,(rtc_bufferCalculated[10]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,44, 18);
	Display_Putc(oled ,(rtc_bufferCalculated[9]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,55, 18);
	Display_Putc(oled ,'.' , &Font_11x18, 1);
	//year (2000-2099)
	Display_GotoXY(oled ,66, 18);
	Display_Putc(oled ,('2') , &Font_11x18, 1);
	Display_GotoXY(oled ,77, 18);
	Display_Putc(oled ,('0') , &Font_11x18, 1);
	Display_GotoXY(oled ,88, 18);
	Display_Putc(oled ,(rtc_bufferCalculated[12]+48) , &Font_11x18, 1);
	Display_GotoXY(oled ,99, 18);
	Display_Putc(oled ,(rtc_bufferCalculated[11]+48) , &Font_11x18, 1);

	Display_UpdateScreen(oled );

}





