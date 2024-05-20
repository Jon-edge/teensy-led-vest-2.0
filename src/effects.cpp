
#include <Arduino.h>
#include <FastLED.h>
#include <cmath>
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

byte currentColor = 0;

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

void circleWave(bool isPalette, uint8_t masterZoom, CRGB color = CRGB::White, uint8_t yOrigin = 7) 
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

void circleWavePalette()
{
	// startup tasks
	if (effectInit == false) {
		effectInit = true;
        currentColor = 0;
	}

    EVERY_N_MILLISECONDS(1000)
    {
        currentColor++;

				while (currentPalette[currentColor].r < 30 && 
					currentPalette[currentColor].g < 30  && 
					currentPalette[currentColor].b < 30 )
				{
					currentColor++;
					if (currentColor > 15) currentColor -= 16;
				}
    }
	circleWave(true, 60, currentPalette[currentColor]);
}

void circleWaveRainbow()
{
	circleWave(false, 15);
}

void squareWave(bool isPalette, uint8_t masterZoom, float angle, CRGB color = CRGB::Black) 
{
    uint8_t timeMultiplier = 2;

    if (isPalette)
    {
        timeMultiplier = 5;
    }

    float radian = angle * PI / 180.0; // Convert angle to radians
    float cosA = cos(radian);
    float sinA = sin(radian);

    // Draw one frame of the animation into the LED array
    for (int y = 0; y < LED_HEIGHT; y++) {
        for (int x = 0; x < LED_WIDTH; x++) {
            // Center the coordinates
            float centeredX = x - LED_WIDTH / 2.0;
            float centeredY = y - LED_HEIGHT / 2.0;

            // Apply rotation transformation
            float newX = centeredX * cosA - centeredY * sinA;
            float newY = centeredX * sinA + centeredY * cosA;

            int v = sin8(static_cast<uint8_t>(fabs(newX * masterZoom)) + static_cast<uint8_t>(fabs(newY * masterZoom)) + mathTime * timeMultiplier);

            if (isPalette)
            {
                leds[XY(x, y)] = color;
                leds[XY(x, y)] -= CRGB(v, v, v);
            }
            else
            {
                leds[XY(x, y)] = CHSV(v, 255, 255);
            }
        }
    }
    mathTimeCounter(false);
}

void squareWaveRainbow(float angle)
{
    squareWave(false, 15, angle);
}

void squareWavePalette(float angle)
{
	// startup tasks
	if (effectInit == false) {
		effectInit = true;
        currentColor = 0;
	}

    EVERY_N_MILLISECONDS(1000)
    {
        currentColor++;
		while (currentPalette[currentColor].r < 30 && 
			currentPalette[currentColor].g < 30  && 
			currentPalette[currentColor].b < 30 )
		{
			currentColor++;
			if (currentColor > 15) currentColor -= 16;
		}
    }
	squareWave(true, 60, angle, currentPalette[currentColor]);
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

void sparkle(uint8_t density, uint8_t colorIndex )
{
	for (int i = 0; i < density; i++)
	{
		int index = random8(NUM_LEDS);
		leds[index] = ColorFromPalette(currentPalette, colorIndex, BRIGHTNESS, NOBLEND);
		colorIndex += 10;
	}
}

// Triple Sine Waves
void threeSine(float angle, 
               uint8_t offsetA, uint8_t offsetB, uint8_t offsetC,
               uint8_t inverseStretch,
               uint8_t inverseWidth) 
{
    uint8_t width = LED_WIDTH;
    uint8_t height = LED_HEIGHT;

    float radian = angle * PI / 180.0; // Convert angle to radians
    float cosA = cos(radian);
    float sinA = sin(radian);

    // Draw one frame of the animation into the LED array
    for (byte x = 0; x < width; x++) {
        for (byte y = 0; y < height; y++) {

            // Calculate "sine" waves with varying periods
            byte sinDistanceR = qmul8(abs(y * (255 / height) - cubicwave8(mathTime * offsetA + x * inverseStretch)), inverseWidth);
            byte sinDistanceG = qmul8(abs(y * (255 / height) - cubicwave8(mathTime * offsetB + x * inverseStretch)), inverseWidth);
            byte sinDistanceB = qmul8(abs(y * (255 / height) - cubicwave8(mathTime * offsetC + x * inverseStretch)), inverseWidth);

            // Apply rotation transformation at the end
            float centeredX = x - width / 2.0;
            float centeredY = y - height / 2.0;
            float newX = centeredX * cosA - centeredY * sinA + width / 2.0;
            float newY = centeredX * sinA + centeredY * cosA + height / 2.0;

            int xi = static_cast<int>(newX);
            int yi = static_cast<int>(newY);

            if (xi >= 0 && xi < width && yi >= 0 && yi < height) {
                leds[XY(xi, yi)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
            }
        }
    }

    mathTime++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle
}

// Triple Sine Waves
void threeSineOg(bool isRotated, 
			   uint8_t offsetA, uint8_t offsetB, uint8_t offsetC,
			   uint8_t inverseStretch,
			   uint8_t inverseWidth) 
{
	uint8_t width = LED_WIDTH;
	uint8_t height = LED_HEIGHT;

	if (isRotated)
	{
		width = LED_HEIGHT;
		height = LED_WIDTH;
	}

	// Draw one frame of the animation into the LED array
	for (byte x = 0; x < width; x++) {
		for (byte y = 0; y < height; y++) {

			// Calculate "sine" waves with varying periods
			// sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
			// qmul8 = p1 * p2, capped at 255
			byte sinDistanceR = qmul8(abs(y * (255 / height) - cubicwave8(mathTime * offsetA + x * inverseStretch)), inverseWidth);
			byte sinDistanceG = qmul8(abs(y * (255 / height) - cubicwave8(mathTime * offsetB + x * inverseStretch)), inverseWidth);
			byte sinDistanceB = qmul8(abs(y * (255 / height) - cubicwave8(mathTime * offsetC + x * inverseStretch)), inverseWidth);

			if (isRotated)
			{
				//CRGB color = ColorFromPalette(currentPalette, index, bri);
				leds[XY(y, x)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
			}
			else
			{
				leds[XY(x, y)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
			}
		}
	}

	mathTime++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle
}



static int cometPosition = LED_HEIGHT - 1;
static unsigned long lastUpdate = 0;
static unsigned long lastFlicker = 0;
static bool flickerState = false;
void meteor(unsigned long currentMillis, int speed, int flickerMs, bool rainbow) {
    Serial.print("Current time: ");
    Serial.println(currentMillis);
    Serial.print("Last update time: ");
    Serial.println(lastUpdate);
    Serial.print("Elapsed time since last update: ");
    Serial.println(currentMillis - lastUpdate);

    // Update comet position based on speed
    if (currentMillis - lastUpdate > (unsigned long)speed) {
        cometPosition--;
        if (cometPosition < 0) cometPosition = LED_HEIGHT - 1; // Reset to the top when it goes out of view
        lastUpdate = currentMillis;
        Serial.print("Updated comet position: ");
        Serial.println(cometPosition);
    }

    // Handle flicker
    if (currentMillis - lastFlicker > (unsigned long)flickerMs) {
        flickerState = !flickerState;
        lastFlicker = currentMillis;
        Serial.print("Flicker state: ");
        Serial.println(flickerState);
    }

    // Clear the LEDs
    fill_solid(leds, LED_WIDTH * LED_HEIGHT, CRGB::Black);

    // Draw the comet
    int middleColumn = LED_WIDTH / 2;
    leds[XY(middleColumn, cometPosition)] = flickerState ? CRGB::White : (rainbow ? ColorFromPalette(RainbowColors_p, cometPosition * 16, 255, LINEARBLEND) : ColorFromPalette(currentPalette, cometPosition * 16, 255, LINEARBLEND));

    // Debug statement for LED drawing
    Serial.print("Drawing comet at: ");
    Serial.print(middleColumn);
    Serial.print(", ");
    Serial.println(cometPosition);
}

const bool SPECIAL_CHARS[][13][9] {
    { // <3 0
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,1,0,0,0,1,0,0},
        {0,0,1,1,0,1,1,0,0},
        {0,0,0,1,1,1,0,0,0},
        {0,0,0,0,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    },
    { // <3 1
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,1,1,0,1,1,0,0},
        {0,1,1,1,1,1,1,1,0},
        {0,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,1,1,1,0,0,0},
        {0,0,0,0,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    },
    { // <3 2
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,1,1,0,0,0,1,1,0},
        {1,1,1,1,0,1,1,1,1},
        {1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,1,1,1,0,0,0},
        {0,0,0,0,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    },
    { // :) 0
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,1,0,0,0,1,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,1,0},
        {0,0,1,0,0,0,1,0,0},
        {0,0,0,1,1,1,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    },
    { // :) 1
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,1,0,0,0,0,0,0},
        {0,0,1,0,0,0,1,1,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,1,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,1,1,1,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    },
    { // :) 2
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0},
        {0,1,1,1,0,0,1,0,0},
        {0,0,0,0,0,0,1,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {1,1,0,0,0,0,0,1,1},
        {0,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    },
    { // :) 3
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,1,0,0,0,1,0,0},
        {0,1,1,1,0,1,1,1,0},
        {0,0,1,0,0,0,1,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {1,0,0,0,0,0,0,0,1},
        {1,1,1,1,0,1,1,1,1},
        {0,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,1,1,1,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    },
    { // :) 4
        {0,0,0,0,0,0,0,0,0},
        {0,0,1,0,0,0,1,0,0},
        {0,1,1,1,0,1,1,1,0},
        {0,1,1,1,0,1,1,1,0},
        {0,1,1,1,0,1,1,1,0},
        {0,0,1,0,0,0,1,0,0},
        {0,0,0,0,0,0,0,0,0},
        {1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,1,1,1,0,0,0},
    },
    { // :) 5
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,1,1,1,0,1,1,1,0},
        {1,1,1,1,1,1,1,1,1},
        {0,1,1,1,0,1,1,1,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,1,1,1,0,0,0},
    },
};



void drawHeart(CRGB color, unsigned long fadeTimeMs)
{
	static uint8_t charIndex = 0;

// Draw the coordinates (without first clearing), flip y axis
	for (uint8_t x = 0; x < 9; x++)
	{
		for (uint8_t y = 0; y < 13; y++)
		{
			if (SPECIAL_CHARS[charIndex][y][x] == true)
			{
				leds[XY(x+4,12-y)] = color;
				leds[XY(x+4,12-y)].nscale8(BRIGHTNESS);
			}
		}
	}

	EVERY_N_MILLIS(fadeTimeMs)
	{
		static int increment = 1;
		charIndex += increment;

		if (charIndex == 2)
		{
			increment = -1;
		}
		else if (charIndex == 0)
		{
			increment = 1;
		}
	}
}


void drawFace(CRGB color, unsigned long fadeTimeMs)
{
	static uint8_t charIndex = 3;

// Draw the coordinates (without first clearing), flip y axis
	for (uint8_t x = 0; x < 9; x++)
	{
		for (uint8_t y = 0; y < 13; y++)
		{
			if (SPECIAL_CHARS[charIndex][y][x] == true)
			{
				leds[XY(x+4,12-y)] = color;
				leds[XY(x+4,12-y)].nscale8(BRIGHTNESS);
			}
		}
	}

	EVERY_N_MILLIS(fadeTimeMs)
	{
		static int increment = 1;
		charIndex += increment;

		if (charIndex == 8)
		{
			increment = -1;
		}
		else if (charIndex == 3)
		{
			increment = 1;
		}
	}
}