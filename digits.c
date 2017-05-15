/* Library used for displaying digits on the screen */

#include <stdint.h>
#include "lcd.h"

uint8_t numDigits = 13;

// Draws a rectangle on the screen
void drawRect(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t col)
{	
	rectangle r;
	r.left = x;
	r.right = x2;
	r.top = y;
	r.bottom = y2;
	fill_rectangle(r, col);
}

// Changes the digit to the corresponding byte array to display
uint8_t digitToByte(uint8_t digit)
{
/*
   Digits and corresponding
    byte 1's for them

     ___1___
	|       |
	|0      |2
	|       | 
	|___3___|
	|       |
	|4      |6
	|       |
	|___5___|
*/

	// Sets toshow to corresponding 
	if(digit == '+')
	{
		return 0b10011000;
	}
	else if(digit == '0')
	{
		return 0b11101110;
	}
	else if(digit == '1')
	{
		return 0b00100010;
	}
	else if(digit == '2')
	{
		return 0b01111100;
	}
	else if(digit == '3')
	{
		return 0b01110110;
	}
	else if(digit == '4')
	{
		return 0b10110010;
	}
	else if(digit == '5')
	{
		return 0b11010110;
	}
	else if(digit == '6')
	{
		return 0b11011110;
	}
	else if(digit == '7')
	{
		return 0b01100010;
	}
	else if(digit == '8')
	{
		return 0b11111110;
	}
	else if(digit == '9')
	{
		return 0b11110010;
	}

	return 0b00000000;
}

void drawDigit(uint8_t digit, int x, int y, int width, int height, uint16_t lineCol)
{
	uint8_t lineWidth = 2;

/*
     ___1___
	|       |
	|0      |2
	|       | 
	|___3___|
	|       |
	|4      |6
	|       |
	|___5___|
*/
	
	// Above shows corresponding bit to display
	uint8_t toShow = digitToByte(digit);
	
	drawRect(x, y, x+width, y+height, BLACK);

	// Upper left
	if(toShow & 0b10000000)
	{
		drawRect(x, y, x + lineWidth, y + (height/2), lineCol);
	}
	// Upper upper
	if(toShow & 0b01000000)
	{
		drawRect(x, y, x + width, y + lineWidth, lineCol);
	}
	// Upper right
	if(toShow & 0b00100000)
	{
		drawRect(x + width - lineWidth, y, x + width, y + (height/2), lineCol);
	}
	// Upper lower / lower upper
	if(toShow & 0b00010000)
	{
		drawRect(x, y + (height/2) - (lineWidth/2),
			x + width, y + (height/2) + (lineWidth/2), lineCol);
	}
	// Lower left
	if(toShow & 0b00001000)
	{
		drawRect(x, y + (height/2), x + lineWidth, y + height, lineCol);
	}
	// Lower lower
	if(toShow & 0b00000100)
	{
		drawRect(x, y + height - lineWidth, x + width, y + height, lineCol);
	}
	// Lower right
	if(toShow & 0b00000010)
	{
		drawRect(x + width - lineWidth, y + (height/2), 
			x + width, y + height, lineCol);
	}
}

// Called to draw number on gui
void showClock(uint8_t* time, uint8_t sel)
{
	static uint8_t oldDigits[13+1] = "-999999999999";

	// Calculates widths and heights
	uint16_t buffer = 20;
	uint16_t colonGap = 25;
	uint16_t midGap = 5;
	uint16_t elementWidths = (LCDHEIGHT - 
		((2 * buffer) + (2 * colonGap) + (3 * midGap))) / numDigits;
	uint16_t elementHeights = elementWidths * 2;

	uint16_t startY = buffer;
	uint16_t startX = buffer;

	uint8_t cnt;
	for(cnt = 0; cnt < numDigits; cnt++)
	{
		// If selected draws
		if(cnt == sel)
		{
			drawDigit(time[cnt], startX, startY,
				elementWidths, elementHeights, LIGHT_PINK);
		}
		else
		{
			// Checks if changed value
			if(time[cnt] != oldDigits[cnt])
			{
				// Value changes so draws
				drawDigit(time[cnt], startX, startY,
					elementWidths, elementHeights, RED);
				oldDigits[cnt] = time[cnt];
			}
		}
	
		startX += midGap + elementWidths;
	}
}
