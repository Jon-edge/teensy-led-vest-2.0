
#include <Arduino.h>
#include <FastLED.h>
#include "main.h"
#include "matrix.h"

//==============COORDS===================
enum Direction
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT,
};

struct Coord {
	int8_t x;
	int8_t y;
};


void RotatingRainbow( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
	byte lineStartHue = startHue8;
	for( byte y = 0; y < LED_HEIGHT; y++) {
		lineStartHue += yHueDelta8;
		byte pixelHue = lineStartHue;      
		for( byte x = 0; x < LED_WIDTH_HALF; x++) {
			pixelHue += xHueDelta8;
			leds[ XYFront(x, y)]  = CHSV( pixelHue, 255, 255);
			leds[ XYBack(x, y)]  = CHSV( pixelHue, 255, 255);
		}
	}
}

unsigned long mathTime = 0; // counter for current position of trig waves
void mathTimeCounter(bool reverse)
{
	if (reverse)
	{
		mathTime++;
		if (mathTime > 4294967000)
		{
			mathTime = 0;
		}
	}
	else
	{
		mathTime--;
		if (mathTime == 0)
		{
			mathTime = 4294967000;
		}
	}
}

void circleWave(bool isPalette, uint8_t masterZoom, CRGB color = CRGB::Black, uint8_t yOrigin = 7) 
{
	uint8_t timeMultiplier = 2;

	if (isPalette)
	{
		timeMultiplier = 5;
	}

	// Draw one frame of the animation into the LED array
    for (int y = 0; y < LED_HEIGHT; y++) {
        for (int x = 0; x < LED_WIDTH_HALF; x++) {
			int vFront = sin8(sqrt( pow(((x-4)*masterZoom),2) + pow(((y-yOrigin)*masterZoom),2))+mathTime*timeMultiplier);
			int vBack = sin8(sqrt( pow(((x-4)*masterZoom),2) + pow(((y-yOrigin)*masterZoom),2))+mathTime*timeMultiplier);

			if (isPalette)
			{
				leds[XYFront(x, y)] = color;
				leds[XYBack(x, y)]  = color;

                leds[XYFront(x, y)] -= CRGB(vFront, vFront, vFront);
                leds[XYBack(x, y)] -= CRGB(vBack, vBack, vBack);
			}
			else
			{
				leds[XYFront(x, y)]  = CHSV(vFront,255,255);
				leds[XYBack(x, y)]  = CHSV(vBack,255,255);
			}
		}
	}
	mathTimeCounter(false);
}

void circleWaveRainbow()
{
	circleWave(false, 15);
}
