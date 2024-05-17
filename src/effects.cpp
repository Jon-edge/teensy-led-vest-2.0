
#include <Arduino.h>
#include <FastLED.h>
#include "main.h"
#include "matrix.h"
#include "palettes.h"

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


#pragma region Rainbow Shit

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

#pragma endregion

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
	for( int i = 0; i < NUM_LEDS; i++) {
		leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
		colorIndex += 3;
	}
}

#pragma region Twinkle
enum { GETTING_DARKER = 0, GETTING_BRIGHTER = 1 };

CRGB makeBrighter( const CRGB& color, fract8 howMuchBrighter) 
{
	CRGB incrementalColor = color;
	incrementalColor.nscale8( howMuchBrighter);
	return color + incrementalColor;
}

CRGB makeDarker( const CRGB& color, fract8 howMuchDarker) 
{
	CRGB newcolor = color;
	newcolor.nscale8( 255 - howMuchDarker);
	return newcolor;
}

uint8_t  directionFlags[ (NUM_LEDS+7) / 8];

bool getPixelDirection( uint16_t i) {
	uint16_t index = i / 8;
	uint8_t  bitNum = i & 0x07;
	return bitRead( directionFlags[index], bitNum);
}

void setPixelDirection( uint16_t i, bool dir) {
	uint16_t index = i / 8;
	uint8_t  bitNum = i & 0x07;
	bitWrite( directionFlags[index], bitNum, dir);
}

void brightenOrDarkenEachPixel( fract8 fadeUpAmount, fract8 fadeDownAmount)
{
	for( uint16_t i = 0; i < NUM_LEDS; i++) {
		if( getPixelDirection(i) == GETTING_DARKER) {
			// This pixel is getting darker
			leds[i] = makeDarker( leds[i], fadeDownAmount);
		} else {
			// This pixel is getting brighter
			leds[i] = makeBrighter( leds[i], fadeUpAmount);
			// now check to see if we've maxxed out the brightness
			if( leds[i].r == 255 || leds[i].g == 255 || leds[i].b == 255) {
				// if so, turn around and start getting darker
				setPixelDirection(i, GETTING_DARKER);
			}
		}
	}
}

#define STARTING_BRIGHTNESS 64
#define FADE_IN_SPEED       50
#define FADE_OUT_SPEED      20
#define DENSITY            255
#define TWINKLE_MULT	     5
void colortwinkles(byte delay)
{
	if ((millis()/10) % (delay/10) == 0)
	{
		// Make each pixel brighter or darker, depending on
		// its 'direction' flag.
		brightenOrDarkenEachPixel( FADE_IN_SPEED, FADE_OUT_SPEED);

		// Now consider adding a new random twinkle
		if( random8() < DENSITY ) {

			for(int i = 0; i<TWINKLE_MULT; i++)
			{
				int pos = random16(NUM_LEDS);
				if( !leds[pos]) {
					leds[pos] = ColorFromPalette( currentPalette, random8(), STARTING_BRIGHTNESS, NOBLEND);
					setPixelDirection(pos, GETTING_BRIGHTER);
				}
			}
		}
	}
}

#pragma endregion

