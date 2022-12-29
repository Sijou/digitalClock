
#include "fonts.h"
#include "stdlib.h"
//#include "string.h"


// I2C address
#ifndef Display_I2C_ADDR
#define Display_I2C_ADDR         0x3C
//#define SSD1306_I2C_ADDR       0x3D	//change address by replacing R on PCB
#endif

// SSD1306 Width and Height in pixels
#define Display_WIDTH            128
#define Display_HEIGHT           64

// available colors
typedef enum {
	Display_COLOR_BLACK = 0x00, // Black color, no pixel
	Display_COLOR_WHITE = 0x01  // Pixel is set, Color depends on OLED
} Display_COLOR_t;



uint8_t Display_Init(void);	//Configure Display

void Display_UpdateScreen(void);	//print data of lines
void Display_Fill(Display_COLOR_t Color);	//entire display is filled white or black
//void Display_ToggleInvert(void);	//change color of pixels

void Display_GotoXY(uint16_t x, uint16_t y);	//set cursor to position x/y
char Display_Putc(char ch, FontDef_t* Font, Display_COLOR_t color);	//character to RAM, pointer to structure
char Display_Puts(char* str, FontDef_t* Font, Display_COLOR_t color);	//string to RAM, pointer to structure
void Display_printTime(uint8_t *rtc_bufferCalculated, FontDef_t *Font11x18);	//time is send to RAM with font size

//void Display_DrawPixel(uint16_t x, uint16_t y, Display_COLOR_t color);	//draw the pixel on position x/y
//void Display_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, Display_COLOR_t c);	//draw line
//void Display_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Display_COLOR_t c);	//draw rectangle
//void Display_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Display_COLOR_t c);	//draw filled rectangle
//void Display_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, Display_COLOR_t color);	//draw triangle
//void Display_DrawCircle(int16_t x0, int16_t y0, int16_t r, Display_COLOR_t c);	//draw circle
//void Display_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, Display_COLOR_t c);	//draw filled circle

void Display_InvertDisplay (int i);	//flip display upside down
void Display_Clear (void);	//entire display is filled black
void Display_DrawBitmap(uint16_t x, uint16_t y, const unsigned char *bitmap, uint16_t w, uint16_t h, Display_COLOR_t color);	//draw a picture in white/black

//void Display_ScrollRight(uint8_t start_row, uint8_t end_row);
//void Display_ScrollLeft(uint8_t start_row, uint8_t end_row);
//void Display_Scrolldiagright(uint8_t start_row, uint8_t end_row);
//void Display_Scrolldiagleft(uint8_t start_row, uint8_t end_row);
//void Display_Stopscroll(void);

// Part of I2C
void Display_I2C_Write(uint8_t address, uint8_t dataRegister, uint8_t data , uint8_t numBytes);	//communication with display, send register and data
void Display_I2C_WriteMulti(uint8_t address, uint8_t dataRegister, uint8_t *data , uint16_t numBytes);	//communication with display, send register and many bytes of data

