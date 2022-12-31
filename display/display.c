//ok
#include "stm32f3xx.h"
#include "display.h"
#include "fonts.h"

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

void Display_UpdateScreen(OLED_t * oled) {
	uint8_t m;

	for (m = 0; m < 8; m++) {
		Display_WriteCommand(oled , 0xB0 + m);
		Display_WriteCommand(oled , 0x00);
		Display_WriteCommand(oled , 0x10);

		Display_I2C_WriteMulti(oled , 0x3C, 0x40, &(oled->Display_Buffer[Display_WIDTH * m]), Display_WIDTH);

	}
}


void Display_Fill(OLED_t * oled , Display_COLOR_t color) {
	// Set memory, fill a block of memory with value
	memset(oled->Display_Buffer, (color == Display_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(oled->Display_Buffer));
  //memset(starting address, value to be filled, number of bytes to be filled)
}


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


void Display_GotoXY(OLED_t * oled ,uint16_t x, uint16_t y) {
	// Set write pointers
	oled->CurrentX = x;
	oled->CurrentY = y;
}


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




void Display_Clear (OLED_t * oled )
{
	Display_Fill (oled , 0);
	Display_UpdateScreen(oled);
}
void Display_ON(OLED_t * oled ) {
	Display_WriteCommand(oled , 0x8D);
	Display_WriteCommand(oled , 0x14);
	Display_WriteCommand(oled , 0xAF);
}
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


void display_printTime(OLED_t *oled ,uint8_t *rtc_bufferCalculated, FontDef_t *Font11x18)
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





