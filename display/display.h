
#include "fonts.h"
#include "stdlib.h"
//#include "string.h"


// SSD1306 Width and Height in pixels 1bit
#define Display_WIDTH            128
#define Display_HEIGHT           64


typedef struct{
	I2C_TypeDef * dev ;
	uint8_t Display_Buffer[Display_WIDTH * Display_HEIGHT / 8];
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
}OLED_t;

// I2C address
#ifndef Display_I2C_ADDR
#define Display_I2C_ADDR         0x3C
//#define SSD1306_I2C_ADDR       0x3D	//change address by replacing R on PCB
#endif


// available colors
typedef enum {
	Display_COLOR_BLACK = 0x00, // Black color, no pixel
	Display_COLOR_WHITE = 0x01  // Pixel is set, Color depends on OLED
} Display_COLOR_t;



uint8_t Display_Init(OLED_t * oled);	//Configure Display

void Display_DrawPixel(OLED_t * oled ,uint16_t x, uint16_t y, Display_COLOR_t color) ;
void Display_UpdateScreen(OLED_t *oled );	//print data of lines
void Display_Fill(OLED_t *oled ,Display_COLOR_t Color);	//entire display is filled white or black
//void Display_ToggleInvert(void);	//change color of pixels

void Display_GotoXY(OLED_t *oled ,uint16_t x, uint16_t y);	//set cursor to position x/y
char Display_Putc(OLED_t *oled ,char ch, FontDef_t* Font, Display_COLOR_t color);	//character to RAM, pointer to structure
char Display_Puts(OLED_t *oled ,char* str, FontDef_t* Font, Display_COLOR_t color);	//string to RAM, pointer to structure
void Display_printTime(OLED_t *oled ,uint8_t *rtc_bufferCalculated, FontDef_t *Font11x18);	//time is send to RAM with font size


void Display_InvertDisplay (OLED_t *oled , int i);	//flip display upside down
void Display_Clear (OLED_t *oled );	//entire display is filled black
void Display_DrawBitmap(OLED_t *oled ,uint16_t x, uint16_t y, const unsigned char *bitmap, uint16_t w, uint16_t h, Display_COLOR_t color);	//draw a picture in white/black


// Part of I2C
void Display_I2C_Write(OLED_t *oled  ,uint8_t address, uint8_t dataRegister, uint8_t data , uint8_t numBytes);	//communication with display, send register and data
void Display_I2C_WriteMulti(OLED_t *oled  , uint8_t address, uint8_t dataRegister, uint8_t *data , uint16_t numBytes);	//communication with display, send register and many bytes of data

