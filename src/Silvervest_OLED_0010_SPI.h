#ifndef Silvervest_OLED_0010_SPI_h
#define Silvervest_OLED_0010_SPI_h

#include <Arduino.h>
#include "Print.h"
#include <inttypes.h>

// since this device requires 10-bit SPI writes, we have to do software SPI using these pins
#define SPI_SCK_PIN			13
#define SPI_MISO_PIN		12
#define SPI_MOSI_PIN		11
#define OLED_DATA			1
#define OLED_COMMAND		0
#define OLED_READ			1
#define OLED_WRITE			0

// main commands
#define OLED_CLEARDISPLAY   0x01
#define OLED_RETURNHOME     0x02
#define OLED_ENTRYMODESET   0x04
#define OLED_DISPLAYCTRL    0x08
#define OLED_CURSORSHIFT    0x10
#define OLED_FUNCTIONSET    0x28
#define OLED_SETCGRAMADDR   0x40
#define OLED_SETDDRAMADDR   0x80

// to control entry mode, pass OLED_ENTRYMODESET OR'd with following flags
#define OLED_ENTRYLEFT      0x02
#define OLED_ENTRYRIGHT     0x00
#define OLED_ENTRYSHIFTINC  0x01
#define OLED_ENTRYSHIFTDEC  0x00

// to control the display, pass OLED_DISPLAYCTRL OR'd with other flags
#define OLED_DISPLAYON      0x04
#define OLED_DISPLAYOFF     0x00
#define OLED_CURSORON       0x02
#define OLED_CURSOROFF      0x00
#define OLED_BLINKON		0x01
#define OLED_BLINKOFF		0x00

// to control display/cursor shifting, pass OLED_CURSORSHIFT OR'd with following flags
#define OLED_SHIFTCURSOR    0x00
#define OLED_SHIFTDISPLAY   0x08
#define OLED_MOVELEFT		0x00
#define OLED_MOVERIGHT      0x04

// to set function, pass OLED_FUNCTIONSET OR'd with following flags
#define OLED_8BITMODE       0x10 // data is sent in 8-bit lengths in DB7..DB0
#define OLED_4BITMODE       0x00  // data is sent in 4-bit lengths in DB7..DB4, in two consecutive writes to make 8-bit data
#define OLED_LANG_EN        0x00
#define OLED_LANG_JP        0x00
#define OLED_LANG_EU1       0x01
#define OLED_LANG_RU        0x02
#define OLED_LANG_EU2       0x03

class Silvervest_OLED_0010_SPI : public Print {
public:
	Silvervest_OLED_0010_SPI(uint8_t enable);

	void init(uint8_t fourbitmode, uint8_t enable);

	void begin(uint8_t cols, uint8_t rows);

	void clear();
	void home();

	void noDisplay();
	void display();
	void noBlink();
	void blink();
	void noCursor();
	void cursor();
	void scrollDisplayLeft();
	void scrollDisplayRight();
	void leftToRight();
	void rightToLeft();
	void autoscroll();
	void noAutoscroll();

	//void setRowOffsets(int row1, int row2, int row3, int row4);
	void createChar(uint8_t, uint8_t[]);
	void setCursor(uint8_t, uint8_t);
	void command(uint8_t);
	
    virtual size_t write(uint8_t);
    virtual size_t write(const char *);

private:
	void send(uint8_t, void *, size_t);
	void sendBit(uint8_t);
	
	uint8_t _enable_pin;

	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
};

#endif
