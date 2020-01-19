/*
 * 16x2 OLED-0010 class driver
 *
 * Derived from LiquidCrystal by David Mellis and should be compatible with the LiquidCrystal library and its examples
 * Init and such derived from datasheet via Adafruit https://cdn-shop.adafruit.com/datasheets/NHD-0216KZW-AB5.pdf
 * 
 * @author Nick Silvestro <nick@silvervest.net>
 * @license BSD License
 */

#include "Silvervest_OLED_0010_SPI.h"

#include <stdio.h>
#include <inttypes.h>
#include "Arduino.h"

Silvervest_OLED_0010_SPI::Silvervest_OLED_0010_SPI(uint8_t enable)
{
	init(0, enable); // 8-bit only for now
}

void Silvervest_OLED_0010_SPI::init(uint8_t fourbitmode, uint8_t enable)
{
	_enable_pin = enable;
	
	if (fourbitmode)
		_displayfunction = OLED_4BITMODE;
	else
		_displayfunction = OLED_8BITMODE;

	// init SPI pins
	pinMode(SPI_SCK_PIN, OUTPUT);
	pinMode(SPI_MISO_PIN, INPUT);
	pinMode(SPI_MOSI_PIN, OUTPUT);
	digitalWrite(_enable_pin, HIGH);
	pinMode(_enable_pin, OUTPUT);
}

// rows and cols are ignore here for now, only supporting 16x2 displays
// but kept for simpler compatibility with LiquidCrystal library
void Silvervest_OLED_0010_SPI::begin(uint8_t cols, uint8_t rows)
{
	// init sequence in 8-bit mode
	// 1. wait a bit, >=1ms on the datasheet
	delayMicroseconds(1000);
	
	// 2. function set
	// we're setting 8-bit mode, english here
	command(OLED_FUNCTIONSET | _displayfunction | OLED_LANG_EN);
	
	// 3. display off
	command(OLED_DISPLAYCTRL | OLED_DISPLAYOFF);
	
	// 4. display clear
	clear();
	
	// 5. entry mode set
	_displaymode = OLED_ENTRYLEFT | OLED_ENTRYSHIFTDEC;
	command(OLED_ENTRYMODESET | _displaymode);
	
	// 6. go home
	home();
	
	// 7. display on
	_displaycontrol = OLED_DISPLAYON | OLED_CURSOROFF;
	command(OLED_DISPLAYCTRL | _displaycontrol);

	// ready!
}

/********** high level commands, for the user! */
void Silvervest_OLED_0010_SPI::clear()
{
	command(OLED_CLEARDISPLAY);
	delayMicroseconds(6000);
	home();
}

void Silvervest_OLED_0010_SPI::home()
{
	command(OLED_RETURNHOME);
}

void Silvervest_OLED_0010_SPI::setCursor(uint8_t col, uint8_t row)
{
	// DDRAM address is 7 bits, first row offset by 0x00 and second offset by 0x40
	// so add the row offset then AND mask the column against 0b01111111 to get our position
	command(OLED_SETDDRAMADDR | (row ? 0x40 : 0x00) | (col & 0x3F));
}

// Turn the display on/off (quickly)
void Silvervest_OLED_0010_SPI::noDisplay()
{
	_displaycontrol &= ~OLED_DISPLAYON;
	command(OLED_DISPLAYCTRL | _displaycontrol);
}
void Silvervest_OLED_0010_SPI::display()
{
	_displaycontrol |= OLED_DISPLAYON;
	command(OLED_DISPLAYCTRL | _displaycontrol);
}

// Turns the underline cursor on/off
void Silvervest_OLED_0010_SPI::noCursor()
{
	_displaycontrol &= ~OLED_CURSORON;
	command(OLED_DISPLAYCTRL | _displaycontrol);
}
void Silvervest_OLED_0010_SPI::cursor()
{
	_displaycontrol |= OLED_CURSORON;
	command(OLED_DISPLAYCTRL | _displaycontrol);
}

// Turn on and off the blinking cursor
void Silvervest_OLED_0010_SPI::noBlink()
{
	_displaycontrol &= ~OLED_BLINKON;
	command(OLED_DISPLAYCTRL | _displaycontrol);
}
void Silvervest_OLED_0010_SPI::blink()
{
	_displaycontrol |= OLED_BLINKON;
	command(OLED_DISPLAYCTRL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void Silvervest_OLED_0010_SPI::scrollDisplayLeft(void)
{
	command(OLED_CURSORSHIFT | OLED_SHIFTDISPLAY | OLED_MOVELEFT);
}
void Silvervest_OLED_0010_SPI::scrollDisplayRight(void)
{
	command(OLED_CURSORSHIFT | OLED_SHIFTDISPLAY | OLED_MOVERIGHT);
}

// This is for text that flows Left to Right
void Silvervest_OLED_0010_SPI::leftToRight(void)
{
	_displaymode |= OLED_ENTRYLEFT;
	command(OLED_ENTRYMODESET | _displaymode);
}
// This is for text that flows Right to Left
void Silvervest_OLED_0010_SPI::rightToLeft(void)
{
	_displaymode &= ~OLED_ENTRYLEFT;
	command(OLED_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void Silvervest_OLED_0010_SPI::autoscroll(void)
{
	_displaymode |= OLED_ENTRYSHIFTINC;
	command(OLED_ENTRYMODESET | _displaymode);
}
// This will 'left justify' text from the cursor
void Silvervest_OLED_0010_SPI::noAutoscroll(void)
{
	_displaymode &= ~OLED_ENTRYSHIFTINC;
	command(OLED_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations with custom characters
void Silvervest_OLED_0010_SPI::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(OLED_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

/*********** mid level commands, for sending data/cmds */
// send a command
inline void Silvervest_OLED_0010_SPI::command(uint8_t value) {
	send(OLED_COMMAND, &value, 1);
}

// write a single character or value
inline size_t Silvervest_OLED_0010_SPI::write(uint8_t value)
{
	send(OLED_DATA, &value, 1);
}

// write a whole string
inline size_t Silvervest_OLED_0010_SPI::write(const char *value)
{
	send(OLED_DATA, &value, strlen(value));
}

/************ low level data pushing commands **********/
// write either a command or data
// this bitbangs the SPI pins to send 10-bits of data, as that's pretty impossible using hardware SPI
// this oled driver needs 10 bits for transmissions: 2-bit header, 8-bit data
// 1st header bit is command or data
// 2nd header bit is read or write
// if we're sending a command, we have to send the header before every data transmission
// if we're sending data, we only have to send it at the beginning of the transmission
void Silvervest_OLED_0010_SPI::send(uint8_t mode, void *buf, size_t count) {
	if (count == 0)
		return;
	
	uint8_t head_sent = 0;

	// select the device to start
	digitalWrite(_enable_pin, LOW);

	// grab a pointer to the first byte off the buffer
	uint8_t *p = (uint8_t *)buf;
	
	do {
		// manually send the first two bits
		// command = LOW, data = HIGH
		if (head_sent == 0) {
			if (mode == OLED_DATA) {
				sendBit(HIGH);
				head_sent = 1; // only send this header once for data transmissions
			} else
				sendBit(LOW);
				
			// read = HIGH, write = LOW
			// we're always writing in this method, so always send write = LOW
			sendBit(LOW);
		}
	
		// send the remaining 8 bits one at a time
		for (uint8_t mask = 0x80; mask; mask >>= 1) {
			sendBit(mask & *p);
		}
		
		// increment the buffer pointer
		p++;
	} while (--count > 0);
	
	// de-select device
	digitalWrite(_enable_pin, HIGH);
}

// clock in a single bit
// according to the data sheet, data is read on the rising edge
void Silvervest_OLED_0010_SPI::sendBit(uint8_t bit)
{
	digitalWrite(SPI_SCK_PIN, LOW);
	digitalWrite(SPI_MOSI_PIN, bit);
	digitalWrite(SPI_SCK_PIN, HIGH);
}