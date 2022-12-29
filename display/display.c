//ok
#include "stm32f3xx.h"
#include "display.h"
#include "fonts.h"

// Write Command/ Data/ Buffer
#define Display_WriteCommand(command)      Display_I2C_Write(0x3C, 0x00, (command), 2)
#define Display_WriteData(data)            Dispaly_I2C_Write(0x3C, 0x40, (data), 2)
//#define SSD1306_WRITECOMMAND(command)      SSD_I2C_Write(0x3C, 0x00, (command), 2)
//#define SSD1306_WRITEDATA(data)            SSD_I2C_Write(0x3C, 0x40, (data), 2)

static uint8_t Display_Buffer[Display_WIDTH * Display_HEIGHT / 8];

// Absolute value (filled Triangle)
#define ABS(x)   ((x) > 0 ? (x) : -(x))


// SSD1306 structure
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

static SSD1306_t Display;

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
void SSD1306_ScrollRight(uint8_t start_row, uint8_t end_row)
{
  SSD1306_WRITECOMMAND (SSD1306_RIGHT_HORIZONTAL_SCROLL);  // send 0x26
  SSD1306_WRITECOMMAND (0x00);  // send dummy
  SSD1306_WRITECOMMAND(start_row);  // start page address
  SSD1306_WRITECOMMAND(0X00);  // time interval 5 frames
  SSD1306_WRITECOMMAND(end_row);  // end page address
  SSD1306_WRITECOMMAND(0X00);
  SSD1306_WRITECOMMAND(0XFF);
  SSD1306_WRITECOMMAND (SSD1306_ACTIVATE_SCROLL); // start scroll
}
*/
/*
void SSD1306_ScrollLeft(uint8_t start_row, uint8_t end_row)
{
  SSD1306_WRITECOMMAND (SSD1306_LEFT_HORIZONTAL_SCROLL);  // send 0x26
  SSD1306_WRITECOMMAND (0x00);  // send dummy
  SSD1306_WRITECOMMAND(start_row);  // start page address
  SSD1306_WRITECOMMAND(0X00);  // time interval 5 frames
  SSD1306_WRITECOMMAND(end_row);  // end page address
  SSD1306_WRITECOMMAND(0X00);
  SSD1306_WRITECOMMAND(0XFF);
  SSD1306_WRITECOMMAND (SSD1306_ACTIVATE_SCROLL); // start scroll
}
*/
/*
void SSD1306_Scrolldiagright(uint8_t start_row, uint8_t end_row)
{
  SSD1306_WRITECOMMAND(SSD1306_SET_VERTICAL_SCROLL_AREA);  // sect the area
  SSD1306_WRITECOMMAND (0x00);   // write dummy
  SSD1306_WRITECOMMAND(SSD1306_HEIGHT);

  SSD1306_WRITECOMMAND(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  SSD1306_WRITECOMMAND (0x00);
  SSD1306_WRITECOMMAND(start_row);
  SSD1306_WRITECOMMAND(0X00);
  SSD1306_WRITECOMMAND(end_row);
  SSD1306_WRITECOMMAND (0x01);
  SSD1306_WRITECOMMAND (SSD1306_ACTIVATE_SCROLL);
}
*/
/*
void SSD1306_Scrolldiagleft(uint8_t start_row, uint8_t end_row)
{
  SSD1306_WRITECOMMAND(SSD1306_SET_VERTICAL_SCROLL_AREA);  // sect the area
  SSD1306_WRITECOMMAND (0x00);   // write dummy
  SSD1306_WRITECOMMAND(SSD1306_HEIGHT);

  SSD1306_WRITECOMMAND(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  SSD1306_WRITECOMMAND (0x00);
  SSD1306_WRITECOMMAND(start_row);
  SSD1306_WRITECOMMAND(0X00);
  SSD1306_WRITECOMMAND(end_row);
  SSD1306_WRITECOMMAND (0x01);
  SSD1306_WRITECOMMAND (SSD1306_ACTIVATE_SCROLL);
}
*/
/*
void SSD1306_Stopscroll(void)
{
	SSD1306_WRITECOMMAND(SSD1306_DEACTIVATE_SCROLL);
}
*/
/*
void SSD1306_InvertDisplay (int i)
{
  if (i) SSD1306_WRITECOMMAND (SSD1306_INVERTDISPLAY);

  else SSD1306_WRITECOMMAND (SSD1306_NORMALDISPLAY);
}
*/

void Display_DrawBitmap(uint16_t x, uint16_t y, const unsigned char *bitmap, uint16_t w, uint16_t h, Display_COLOR_t color)
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
            	Display_DrawPixel(x+i, y, color);
        }
    }
}




uint8_t Display_Init(void) {

	//delay
	uint32_t p = 2500;
	while(p>0)
		p--;

	Display_WriteCommand(0xAE); //display off
	Display_WriteCommand(0x20); //Set Memory Addressing Mode
	Display_WriteCommand(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	Display_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	Display_WriteCommand(0xC8); //Set COM Output Scan Direction
	Display_WriteCommand(0x00); //---set low column address
	Display_WriteCommand(0x10); //---set high column address
	Display_WriteCommand(0x40); //--set start line address
	Display_WriteCommand(0x81); //--set contrast control register
	Display_WriteCommand(0xFF);
	Display_WriteCommand(0xA1); //--set segment re-map 0 to 127
	Display_WriteCommand(0xA6); //--set normal display
	Display_WriteCommand(0xA8); //--set multiplex ratio(1 to 64)
	Display_WriteCommand(0x3F); //
	Display_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Display_WriteCommand(0xD3); //-set display offset
	Display_WriteCommand(0x00); //-not offset
	Display_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
	Display_WriteCommand(0xF0); //--set divide ratio
	Display_WriteCommand(0xD9); //--set pre-charge period
	Display_WriteCommand(0x22); //
	Display_WriteCommand(0xDA); //--set Com pins hardware configuration
	Display_WriteCommand(0x12);
	Display_WriteCommand(0xDB); //--set vcomh
	Display_WriteCommand(0x20); //0x20,0.77xVcc
	Display_WriteCommand(0x8D); //--set DC-DC enable
	Display_WriteCommand(0x14); //
	Display_WriteCommand(0xAF); //--turn on SSD1306 panel


	Display_WriteCommand(Display_DEACTIVATE_SCROLL);
	//Display_Fill(Display_COLOR_WHITE);	// Clear screen
	Display_Fill(Display_COLOR_BLACK);	// Clear screen
	Display_UpdateScreen();		// Update screen

	// Set default values
	Display.CurrentX = 0;
	Display.CurrentY = 0;

	// Initialized OK
	Display.Initialized = 1;

	// Return OK
	return 1;
}

void Display_UpdateScreen(void) {
	uint8_t m;

	for (m = 0; m < 8; m++) {
		Display_WriteCommand(0xB0 + m);
		Display_WriteCommand(0x00);
		Display_WriteCommand(0x10);

		Display_I2C_WriteMulti(0x3C, 0x40, &Display_Buffer[Display_WIDTH * m], Display_WIDTH);

	}
}
/*
void SSD1306_ToggleInvert(void) {
	uint16_t i;

	// Toggle invert
	SSD1306.Inverted = !SSD1306.Inverted;

	// Do memory toggle
	for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
		SSD1306_Buffer[i] = ~SSD1306_Buffer[i];
	}
}
*/

void Display_Fill(Display_COLOR_t color) {
	// Set memory, fill a block of memory with value
	memset(Display_Buffer, (color == Display_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(Display_Buffer));
  //memset(starting address, value to be filled, number of bytes to be filled)
}


void Display_DrawPixel(uint16_t x, uint16_t y, Display_COLOR_t color) {

	if (x >= Display_WIDTH || y >= Display_HEIGHT)
	{
		// Error
		return;
	}

	// Check if pixels are inverted
	if (Display.Inverted) {
		color = (Display_COLOR_t)!color;
	}

	// Set color
	if (color == Display_COLOR_WHITE) {
		Display_Buffer[x + (y / 8) * Display_WIDTH] |= 1 << (y % 8);
	} else {
		Display_Buffer[x + (y / 8) * Display_WIDTH] &= ~(1 << (y % 8));
	}
}


void Display_GotoXY(uint16_t x, uint16_t y) {
	// Set write pointers
	Display.CurrentX = x;
	Display.CurrentY = y;
}


char Display_Putc(char ch, FontDef_t *Font, Display_COLOR_t color) {
	uint32_t i, b, j;

	// Check available space in LCD
	if (
		Display_WIDTH <= (Display.CurrentX + Font->FontWidth) ||
		Display_HEIGHT <= (Display.CurrentY + Font->FontHeight)
	) {
		// Error
		return 0;
	}

	// Go through font
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				Display_DrawPixel(Display.CurrentX + j, (Display.CurrentY + i), (Display_COLOR_t) color);
			} else {
				Display_DrawPixel(Display.CurrentX + j, (Display.CurrentY + i), (Display_COLOR_t)!color);
			}
		}
	}

	// Increase pointer
	Display.CurrentX += Font->FontWidth;

	// Return character written
	return ch;
}


char Display_Puts(char* str, FontDef_t* Font, Display_COLOR_t color) {
	// Write characters
	while (*str) {
		// Write character by character
		if (Display_Putc(*str, Font, color) != *str) {
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
void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c) {

	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	// Check for overflow
	if (x0 >= SSD1306_WIDTH) {
		x0 = SSD1306_WIDTH - 1;
	}
	if (x1 >= SSD1306_WIDTH) {
		x1 = SSD1306_WIDTH - 1;
	}
	if (y0 >= SSD1306_HEIGHT) {
		y0 = SSD1306_HEIGHT - 1;
	}
	if (y1 >= SSD1306_HEIGHT) {
		y1 = SSD1306_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		// Vertical line
		for (i = y0; i <= y1; i++) {
			SSD1306_DrawPixel(x0, i, c);
		}

		// Return from function
		return;
	}

	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		// Horizontal line
		for (i = x0; i <= x1; i++) {
			SSD1306_DrawPixel(i, y0, c);
		}

		// Return from function
		return;
	}

	while (1) {
		SSD1306_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}
*/
/*
void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	// Check input parameters
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		// Return error
		return;
	}

	// Check Width and Height
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}

	// Draw 4 lines
	SSD1306_DrawLine(x, y, x + w, y, c);         // Top line
	SSD1306_DrawLine(x, y + h, x + w, y + h, c); // Bottom line
	SSD1306_DrawLine(x, y, x, y + h, c);         // Left line
	SSD1306_DrawLine(x + w, y, x + w, y + h, c); // Right line
}
*/
/*
void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	uint8_t i;

	// Check input parameters
	if (
		x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT
	) {
		// Return error
		return;
	}

	// Check width and height
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}

	// Draw lines
	for (i = 0; i <= h; i++) {
		SSD1306_DrawLine(x, y + i, x + w, y + i, c);
	}
}
*/
/*
void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	// Draw lines
	SSD1306_DrawLine(x1, y1, x2, y2, color);
	SSD1306_DrawLine(x2, y2, x3, y3, color);
	SSD1306_DrawLine(x3, y3, x1, y1, color);
}
*/
/*
void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {

	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0,
			numpixels = 0, curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		SSD1306_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}
*/
/*
void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, c);
    SSD1306_DrawPixel(x0, y0 - r, c);
    SSD1306_DrawPixel(x0 + r, y0, c);
    SSD1306_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawPixel(x0 + x, y0 + y, c);
        SSD1306_DrawPixel(x0 - x, y0 + y, c);
        SSD1306_DrawPixel(x0 + x, y0 - y, c);
        SSD1306_DrawPixel(x0 - x, y0 - y, c);

        SSD1306_DrawPixel(x0 + y, y0 + x, c);
        SSD1306_DrawPixel(x0 - y, y0 + x, c);
        SSD1306_DrawPixel(x0 + y, y0 - x, c);
        SSD1306_DrawPixel(x0 - y, y0 - x, c);
    }
}
*/
/*
void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, c);
    SSD1306_DrawPixel(x0, y0 - r, c);
    SSD1306_DrawPixel(x0 + r, y0, c);
    SSD1306_DrawPixel(x0 - r, y0, c);
    SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}
*/


void Display_Clear (void)
{
	Display_Fill (0);
	Display_UpdateScreen();
}
void Display_ON(void) {
	Display_WriteCommand(0x8D);
	Display_WriteCommand(0x14);
	Display_WriteCommand(0xAF);
}
void Display_OFF(void) {
	Display_WriteCommand(0x8D);
	Display_WriteCommand(0x10);
	Display_WriteCommand(0xAE);
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
void ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	uint8_t dt[256];
	dt[0] = reg;
	uint8_t i;
	for(i = 0; i < count; i++)
	dt[i+1] = data[i];
	//HAL_I2C_Master_Transmit(&hi2c1, address, dt, count+1, 10);
}
*/
/*
void ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) {
	uint8_t dt[2];
	dt[0] = reg;
	dt[1] = data;

	//HAL_I2C_Master_Transmit(&hi2c1, address, dt, 2, 10);
}
*/


void Display_I2C_WriteMulti(uint8_t address, uint8_t dataRegister, uint8_t *data , uint16_t numBytes)	//data is a pointer
{
	uint8_t i;
	uint8_t j;
	uint8_t dt[numBytes];		//necessary for multi Write

	I2C1->CR2 = ((address << 1) << I2C_CR2_SADD_Pos) | (numBytes << I2C_CR2_NBYTES_Pos) | (0 << I2C_CR2_RD_WRN_Pos | I2C_CR2_AUTOEND);	//send address + data of register + WRITE
	I2C1->CR2 |= I2C_CR2_START;
	while ((I2C1->ISR & I2C_ISR_TXIS)==0) {	//TXDR is empty
		if ((I2C1->ISR & I2C_ISR_NACKF)){
			return;}}

	dt[0] = dataRegister;
	for(i=0; i<numBytes; i++)
	{
	dt[i+1] = data[i];	//necessary for multi Write
	}

	for(j=0; j <= (numBytes-1); j++)
	{
		I2C1->TXDR = dt[j];
		if(j < (numBytes-1))
		{
			while((I2C1->ISR & I2C_ISR_TXIS)==0){}		//ggf TXIS mit TXE ergänzen
		}
	}
}


void Display_I2C_Write(uint8_t address, uint8_t dataRegister, uint8_t data , uint8_t numBytes)
{
	uint8_t j;
	uint8_t dt[2];

	dt[0] = dataRegister;
	dt[1] = data;

	I2C1->CR2 = ((address << 1) << I2C_CR2_SADD_Pos) | (numBytes << I2C_CR2_NBYTES_Pos) | (0 << I2C_CR2_RD_WRN_Pos | I2C_CR2_AUTOEND);	//send address + data of register + WRITE
	I2C1->CR2 |= I2C_CR2_START;
	while ((I2C1->ISR & I2C_ISR_TXIS)==0) {	//TXDR is empty
		if ((I2C1->ISR & I2C_ISR_NACKF)){
			return;}}

	for(j=0; j <= (numBytes-1); j++)
	{
		I2C1->TXDR = dt[j];
		if(j < (numBytes-1))
		{
			while((I2C1->ISR & I2C_ISR_TXIS)==0){}
		}
	}
}


void display_printTime(uint8_t *rtc_bufferCalculated, FontDef_t *Font11x18)
{
	//seconds
	Display_GotoXY(0, 0);
	Display_Putc((rtc_bufferCalculated[5]+48) , &Font_11x18, 1);
	Display_GotoXY(11, 0);
	Display_Putc((rtc_bufferCalculated[4]+48) , &Font_11x18, 1);
	Display_GotoXY(22, 0);
	Display_Putc(':' , &Font_11x18, 1);
	//minutes
	Display_GotoXY(33, 0);
	Display_Putc((rtc_bufferCalculated[3]+48) , &Font_11x18, 1);
	Display_GotoXY(44, 0);
	Display_Putc((rtc_bufferCalculated[2]+48) , &Font_11x18, 1);
	Display_GotoXY(55, 0);
	Display_Putc(':' , &Font_11x18, 1);
	//hours
	Display_GotoXY(66, 0);
	Display_Putc((rtc_bufferCalculated[1]+48) , &Font_11x18, 1);
	Display_GotoXY(77, 0);
	Display_Putc((rtc_bufferCalculated[0]+48) , &Font_11x18, 1);
	//date
	Display_GotoXY(0, 18);
	Display_Putc((rtc_bufferCalculated[8]+48) , &Font_11x18, 1);
	Display_GotoXY(11, 18);
	Display_Putc((rtc_bufferCalculated[7]+48) , &Font_11x18, 1);
	Display_GotoXY(22, 18);
	Display_Putc('.' , &Font_11x18, 1);
	//month
	Display_GotoXY(33, 18);
	Display_Putc((rtc_bufferCalculated[10]+48) , &Font_11x18, 1);
	Display_GotoXY(44, 18);
	Display_Putc((rtc_bufferCalculated[9]+48) , &Font_11x18, 1);
	Display_GotoXY(55, 18);
	Display_Putc('.' , &Font_11x18, 1);
	//year (2000-2099)
	Display_GotoXY(66, 18);
	Display_Putc(('2') , &Font_11x18, 1);
	Display_GotoXY(77, 18);
	Display_Putc(('0') , &Font_11x18, 1);
	Display_GotoXY(88, 18);
	Display_Putc((rtc_bufferCalculated[12]+48) , &Font_11x18, 1);
	Display_GotoXY(99, 18);
	Display_Putc((rtc_bufferCalculated[11]+48) , &Font_11x18, 1);

	Display_UpdateScreen();

}





