#include <Arduino.h>
#include <FastLED.h>
#include "matrix.h"

// max dimensions
const uint8_t LED_WIDTH = 17;
const uint8_t LED_WIDTH_HALF = 9;
const uint8_t LED_HEIGHT = 13;

//   SubStrip index       0  1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
//   Direction            u  d   u   d   u   d   u   d   u   d   u   d   u  *u   d   u   d
uint8_t len[17] =       { 7, 9, 10, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 10,  9,  7}; // length of each strip
uint8_t preDeadZone[17]={ 2, 3,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  1,  4};

// Control
uint8_t MODE = 0;
uint8_t BRIGHTNESS = 255;
uint8_t brightness[9] = {30, 50, 75, 100, 125, 150, 175, 200, 255};
uint8_t brightnessIndex = 6;
uint8_t textIndex = 0;

CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* leds( leds_plus_safety_pixel + 1);

int XY(uint8_t x, uint8_t y)
{
	//initial boundry check
	if ((x > LED_WIDTH) ||
		(y > LED_HEIGHT))
	{
		return -1;
	}

	uint8_t stripIndex = 0;  // overall position on strip as a whole
	uint8_t lastSegment = 0;  // Position on substrip after previous strips accounted for

	// Back portion
	if ((x > 3) && (x < 13))
	{
		if( x & 0x01) {
			// Odd rows run backwards
			lastSegment = (LED_HEIGHT - 1) - y;
		} else {
			// Even rows run forwards

			lastSegment = y;
		}
	}

	// Special handling for last segment for trapezoidal front panels
	else
	{
		// Diagonal deadzones
		if ( (((x==0)||(x==16)) && ((y<2)||(y>8))) ||
			(((x==1)||(x==15)) && ((y<1)||(y>9))) ||
			(((x==2)||(x==14)) && ((y<1)||(y>10))) ||
			(((x==3)||(x==13)) && (y>11)) )
		{
			return -1;
		}

		if(x & 0x01) {
			if (x<4)
			{
				// Odd rows run backwards on left
				lastSegment = (LED_HEIGHT - 1) - y;
			}
			else
			{
				// forwards on right
				lastSegment = y;
			}
		}
		else
		{
			if (x<4)
			{
				// Even rows run forwards on right
				lastSegment = y;
			}
			else
			{
				// backwards on left
				lastSegment = (LED_HEIGHT - 1) - y;
			}
		}

		// Subtract deadzone
		lastSegment -= preDeadZone[x];
	}

	// Collect index total as we move across the X axis
	if (x>0)
	{
		for (int i = 0; i < x; i++)
		{
			stripIndex += len[i];
		}
	}

	// Add final partial strip
	stripIndex += lastSegment;

	return stripIndex;
}

int XYBack(uint8_t x, uint8_t y)
{
	//initial boundry check
	if ((x > 8) || 
		(x < 0) ||
		(y > LED_HEIGHT) ||
		(y < 0))
	{
		return -1;
	}

	return XY(x+4, y);
}

int XYFront(uint8_t x, uint8_t y)
{
  //initial boundry check
  if ((x > 8) || (x == 4) || (y > LED_HEIGHT))
  {
    return -1;
  }

  if (x < 4)
  {
    return XY(x+13,y);
  }
  else
  {
    return XY(x-5, y);
  }
	
}

void fillLeds(CRGB color)
{
    for( int i = 0; i < NUM_LEDS; i++) {
		leds[i] = color;
	}
}

void clearLeds()
{
	fillLeds(CRGB::Black);
}

void fadeAll(byte fadeIncr) {
	for (byte i = 0; i < NUM_LEDS; i++) {
		leds[i] = leds[i].fadeToBlackBy(fadeIncr);
	}
}