#include <FastLED.h>
#include "lightVestFastLED.h"
#include "palettes.h"
#include "effects.h"
#include "font.h"
#include <Arduino.h>

void setup() {
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
	//set_max_power_in_volts_and_milliamps(5, 2000);
	pinMode(ledPin, OUTPUT);
    pinMode(modeButtonPin, INPUT_PULLUP);
    pinMode(paletteButtonPin, INPUT_PULLUP);
	digitalWrite(ledPin, HIGH);

	// PALETTE
	currentPalette = RainbowColors_p;
	currentBlending = LINEARBLEND;

	//testing
	Serial.begin(9600);
	Serial.println("NeonVest");
	Serial.print("Initial mode: ");
	Serial.println(MODE);
}

double test = 10;

void loop()
{
	// Handle Serial commands:
	if (Serial.available() > 0) {
		MODE = Serial.read() - '0';
		Serial.print("Entering mode ");
		Serial.println(MODE);
		/*test = Serial.readString().toFloat();
		Serial.println(test);*/
	}

	unsigned long ms = millis();
	static uint8_t colorIndex = 0; // Palette vars
	static uint8_t startIndex = 0;

    paletteButtonLogic();
	modeButtonLogic();

	selectPalette(currentPaletteIndex);

	BRIGHTNESS = brightness[brightnessIndex];
	FastLED.setBrightness( BRIGHTNESS );

	if (isAutoModeCycle)
	{
		EVERY_N_SECONDS(5)
		{
			MODE++;
			effectInit = false;
			Serial.print("(AUTOCYCLE) Entering mode ");
			Serial.println(MODE);
		}
	}

	EVERY_N_SECONDS(5)
	{
		ChangePalettePeriodically();
	}

	switch (MODE)
	{
	case 0: 
		{
			sinusoid1(false);
		}
		break;
	case 1:// (Chasing colors) Palette example
	{
		EVERY_N_MILLISECONDS(10)
		{
			startIndex = startIndex + 1; /* motion speed */
		}
		FillLEDsFromPaletteColors(startIndex);
	}
		break;

	case 2: 

		// Twinkle
		{
			colortwinkles(20);
		}
		break;

	case 3:
		{
            rainbowWithGlitter();
              EVERY_N_MILLISECONDS( 1 ) { gHue++; } // slowly cycle the "base color" through the rainbow
               FastLED.delay(1000/120); 
            /*
			EVERY_N_MILLISECONDS(150)
			{
				halfSneks();
			}
			EVERY_N_MILLISECONDS(1)
			{
				fadeAll(1);
			}*/

		}
		break;

	//case 4: // Rotating vertical bars
		//{
			//// Vertical bars
			//static uint8_t barPos = 0;

			//EVERY_N_SECONDS(7)
			//{
			//	ChangePalettePeriodically();
			//}
			//EVERY_N_MILLISECONDS(45)
			//{
			//	clearLeds();

			//	colorIndex += 2;
			//	drawBar(16-barPos, colorIndex, BRIGHTNESS);
			//	drawBar(16-(barPos-1), colorIndex, BRIGHTNESS-100);
			//	drawBar(16-(barPos-2), colorIndex, BRIGHTNESS-150);
			//	drawBar(16-(barPos-3), colorIndex, BRIGHTNESS-200);
			//	drawBar(barPos, colorIndex, BRIGHTNESS);
			//	drawBar(barPos-1, colorIndex, BRIGHTNESS-100);
			//	drawBar(barPos-2, colorIndex, BRIGHTNESS-150);
			//	FastLED.show(); 
			//	barPos++;
			//	if (barPos > 17)
			//		barPos = 0;
			//}
		//}
		//break;

	case 4: // Sparkle (face)
		{
			EVERY_N_MILLISECONDS(3)
			{
				colorIndex++;
				clearLeds();
				sparkle(7, colorIndex);
			}
		}
		break;

	case 5: // Sparkle (medium + heart)
		{
			EVERY_N_MILLISECONDS(3)
			{
				colorIndex++;
				clearLeds();
				sparkle(20, colorIndex);
			}

			//drawHeart(heartColor, 100);
		}
		break;

	case 6: // Noise
		{
				// generate noise data
			fillnoise8();

				// convert the noise data to colors in the LED array
				// using the current palette
			mapNoiseToLEDsUsingPalette();

		}
		break;

	case 7: // Sin Wave (Horizontal)
		{
			EVERY_N_MILLISECONDS(50)
			{
				threeSine(false, 8, 9, 10, 6.5, 7.5);
			}
		}
		break;

	case 8: // Sin Wave (Vertical)
		{
			EVERY_N_MILLISECONDS(60)
			{
				threeSine(true, 8, 9, 10, 4, 9);
			}
		}
		break;

	case 9:
		{
			circleWavePalette();
		}
		break;

	case 10:
		{
			circleWaveRainbow();
		}
		break;
	case 11:
	{
		// Rotating rainbow
		int32_t yHueDelta32 = ((int32_t)sin16(ms * (27 / 1)) * (350 / LED_WIDTH));
		int32_t xHueDelta32 = ((int32_t)sin16(ms * (39 / 1)) * (310 / LED_HEIGHT));
		RotatingRainbow(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
	}
	break;
	case 12:
	{
        juggle();
		EVERY_N_MILLISECONDS( 1 ) { gHue++; } // slowly cycle the "base color" through the rainbow
	}
	break;
	default:
		{
			Serial.println("Unknown mode, resetting to mode 0");
			MODE = 0;
		}
		break;
	}

	switch (textIndex)
	{
		case 0:
		{
			//no text
		}
		break;
		case 1:
		{
			drawScrollString("EDC GET LIT!  ", 7, CRGB::White, 140);
		}
		break;
		case 2:
		{
			drawScrollString("FREE LIGHTSHOW!  ", 7, CRGB::White, 140);
		}
		break;
		case 3:
		{
			drawScrollString("EAT SLEEP RAVE REPEAT!  ", 7, CRGB::White, 140);
		}
		break;
		case 4:
		{
			drawScrollString("LIT AF!  TURN UP!  ", 7, CRGB::White, 140);
		}
		break;
		case 5:
		{
			drawScrollString("FUCK IT UP!  AY AY AYY  ", 7, CRGB::White, 140);
		}
		break;
		case 6:
		{
			drawScrollString("PLUR AF  PLUR AF  ", 7, CRGB::White, 140);
		}
		break;
		case 7:
		{
			drawScrollString("WUB WUB WUB ", 7, CRGB::White, 140);
		}
		break;
		case 8:
		{
			drawScrollString("BRUHH!  BRUHH!  ", 7, CRGB::White, 140);
		}
		break;
		case 9:
		{
			CRGB heartColor = CRGB::White;
			EVERY_N_MILLISECONDS(3)
			{
				switch (random8(0, 8))
				{
				case 0:
					heartColor = CRGB::White;
					break;
				case 1:
				case 2:
				case 3:
				case 4:
					heartColor = CRGB::Red;
					break;
				case 5:
					heartColor = CRGB::Pink;
					break;
				case 6:
					heartColor = CRGB::Salmon;
					break;
				case 7:
					heartColor = CRGB::Purple;
					break;
				}
			}

			drawHeart(heartColor, 100);
		}
		break;
		case 10:
		{
			CRGB textColor = CRGB::White;
			if ((currentPalette[0].r == currentPalette[0].g == currentPalette[0].b > 200) &&
				(currentPalette[4].r == currentPalette[4].g == currentPalette[4].b > 200) &&
				(currentPalette[8].r == currentPalette[8].g == currentPalette[8].b > 200) &&
				(currentPalette[12].r == currentPalette[12].g == currentPalette[12].b > 200))
			{
				textColor = CRGB::Red;
			}
			drawFace(textColor, 100);
		}
		break;
		default:
		{
			Serial.println("Unknown mode, resetting to mode 0");
			MODE = 0;
		}
		break;
	}

	FastLED.show();
	
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < 17; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, 17, 20);
  byte dothue = 0;
  for( int i = 0; i < 5; i++) {
    leds[beatsin16(i+7,0,17)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, 17, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(17) ] += CRGB::White;
  }
}


const uint8_t NUM_FRONT_SNEKS = 4;
const uint8_t NUM_BACK_SNEKS = 5;
static Snek * snekPitFront[NUM_FRONT_SNEKS];
static Snek * snekPitBack[NUM_BACK_SNEKS];
void halfSneks()
{
	// startup tasks
	if (effectInit == false) {
		effectInit = true;
		//effectDelay = 50;
		currentColor = 0;
		for (uint8_t i = 0; i < NUM_FRONT_SNEKS; i++)
		{
			snekPitFront[i] = new Snek(currentPalette[i], LED_WIDTH_HALF-1, LED_HEIGHT);
		}
		for (uint8_t i = 0; i < NUM_BACK_SNEKS; i++)
		{
			snekPitBack[i] = new Snek(currentPalette[i], LED_WIDTH_HALF, LED_HEIGHT);
		}
	}

	// draw new snek head
	for (uint8_t i = 0; i < NUM_FRONT_SNEKS; i++)
	{
		// Color sneks with evenly distributed colors, omit black.
		uint8_t index = i * 4;
		CRGB color = currentPalette[index];
		while (color.r < 30 && 
			color.g < 30  && 
			color.b < 30 )
		{
			index++;
			color = currentPalette[index];
		}
		snekPitFront[i]->color = color;
		snekPitFront[i]->move(2);
		leds[XYFront(snekPitFront[i]->curX, snekPitFront[i]->curY, true)] = snekPitFront[i]->color;
	}

	// draw new snek head
	for (uint8_t i = 0; i < NUM_BACK_SNEKS; i++)
	{
		// Color sneks with evenly distributed colors, omit black.
		uint8_t index = i * 2.6;
		CRGB color = currentPalette[index];
		while (color.r < 30 && 
			color.g < 30  && 
			color.b < 30 )
		{
			index++;
			color = currentPalette[index];
		}
		snekPitBack[i]->color = color;

		snekPitBack[i]->move(2);
		leds[XYBack(snekPitFront[i]->curX, snekPitBack[i]->curY)] = snekPitBack[i]->color;
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

void circleWaveCostume()
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
			currentPalette[currentColor].g < 30 &&
			currentPalette[currentColor].b < 30)
		{
			currentColor++;
			if (currentColor > 15) currentColor -= 16;
		}
	}
	circleWave(true, 60, currentPalette[currentColor], 1);
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
	// startup tasks
	if (effectInit == false) {
		effectInit = true;
	}
	circleWave(false, 15);
}

// Sinusoid 1 (lines)
void sinusoid1(bool reverse)
{
	// inverse zoom
	double masterZoom = 0.15;
	double zoomWave = 10;
	double rotationSpeed = 2;
	double timeMult = 10;

	// Draw one frame of the animation into the LED array
	for (int y = 0; y < LED_HEIGHT; y++) {
		for (int x = 0; x < LED_WIDTH_HALF; x++) {
			int v = sin8(masterZoom* (y*sin8((mathTime*timeMult)/zoomWave) + x*cos8((double)(mathTime*timeMult)/rotationSpeed))+ mathTime);

			//int v = sin8(masterZoom*(y - x + 10));

			leds[XYFront(x, y)]  = CHSV(v, 255, 255-v);
			leds[XYBack(x, y)] = CHSV(v, 255, 255-v);//CRGB(v,0,0);

			//leds[XYFront(x, y)] = CHSV(currentPalette[], 255, v);
			//leds[XYBack(x, y)] = CHSV(currentPalette[], 255, v);
		}
		//Serial.println(cos8(mathTime/rotationSpeed));
	}
	mathTimeCounter(reverse);
}

// Triple Sine Waves
void threeSine(bool isRotated, 
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

// Noise
uint8_t       colorLoop = 1;

// The 16 bit version of our coordinates
static uint16_t xNoise = random16();
static uint16_t yNoise = random16();
static uint16_t zNoise = random16();
uint16_t speed = 1; // speed is set dynamically once we've started up
uint16_t scale = 30; // scale is set dynamically once we've started up
uint8_t noise[LED_WIDTH][LED_WIDTH];

//// Fill the x/y array of 8-bit noise values using the inoise8 function.
//void fillnoise8() 
//{
//	// If we're runing at a low "speed", some 8-bit artifacts become visible
//	// from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
//	// The amount of data smoothing we're doing depends on "speed".
//	uint8_t dataSmoothing = 0;
//	if( speed < 50) {
//		dataSmoothing = 200 - (speed * 4);
//	}
//
//	for(int i = 0; i < LED_WIDTH; i++) {
//		int ioffset = scale * i;
//		for(int j = 0; j < LED_WIDTH; j++) {
//			Serial.println("Test");
//			int joffset = scale * j;
//
//			uint8_t data = inoise8(xNoise + ioffset, yNoise + joffset, zNoise);
//
//			// The range of the inoise8 function is roughly 16-238.
//			// These two operations expand those values out to roughly 0..255
//			// You can comment them out if you want the raw noise data.
//			data = qsub8(data,16);
//			data = qadd8(data,scale8(data,39));
//
//			if( dataSmoothing ) {
//				uint8_t olddata = noise[i][j];
//				uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
//				data = newdata;
//			}
//
//			noise[i][j] = data;
//		}
//	}
//
//	zNoise += speed;
//
//	// apply slow drift to X and Y, just for visual variation.
//	xNoise += speed / 8;
//	yNoise -= speed / 16;
//}
//
//void mapNoiseToLEDsUsingPalette()
//{
//	static uint8_t ihue=0;
//
//	for(int i = 0; i < LED_WIDTH; i++) {
//		for(int j = 0; j < LED_HEIGHT; j++) {
//			// We use the value at the (i,j) coordinate in the noise
//			// array for our brightness, and the flipped value from (j,i)
//			// for our pixel's index into the color palette.
//
//			uint8_t index = noise[j][i];
//			uint8_t bri =   noise[i][j];
//
//			// if this palette is a 'loop', add a slowly-changing base value
//			if( colorLoop) { 
//				index += ihue;
//			}
//
//			// brighten up, as the color palette itself often contains the 
//			// light/dark dynamic range desired
//			if( bri > 127 ) {
//				bri = 255;
//			} else {
//				bri = dim8_raw( bri * 2);
//			}
//
//				leds[XY(i, j)] = ColorFromPalette(currentPalette, index, bri);;
//		}
//	}
//
//	ihue+=1;
//}

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
	// If we're runing at a low "speed", some 8-bit artifacts become visible
	// from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
	// The amount of data smoothing we're doing depends on "speed".
	uint8_t dataSmoothing = 0;
	if (speed < 50) {
		dataSmoothing = 200 - (speed * 4);
	}

	for (int i = 0; i < LED_WIDTH; i++) {
		int ioffset = scale * i;
		for (int j = 0; j < LED_WIDTH; j++) {
			int joffset = scale * j;

			uint8_t data = inoise8(xNoise + ioffset, yNoise + joffset, zNoise);

			// The range of the inoise8 function is roughly 16-238.
			// These two operations expand those values out to roughly 0..255
			// You can comment them out if you want the raw noise data.
			data = qsub8(data, 16);
			data = qadd8(data, scale8(data, 39));

			if (dataSmoothing) {
				uint8_t olddata = noise[i][j];
				uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
				data = newdata;
			}

			noise[i][j] = data;
		}
	}

	zNoise += speed;

	// apply slow drift to X and Y, just for visual variation.
	xNoise += speed / 8;
	yNoise -= speed / 16;
}

void mapNoiseToLEDsUsingPalette()
{
	static uint8_t ihue = 0;

	for (int i = 0; i < LED_WIDTH_HALF; i++) {
		for (int j = 0; j < LED_HEIGHT; j++) {
			// We use the value at the (i,j) coordinate in the noise
			// array for our brightness, and the flipped value from (j,i)
			// for our pixel's index into the color palette.

			uint8_t index = noise[j][i];
			uint8_t bri = noise[i][j];

			// if this palette is a 'loop', add a slowly-changing base value
			if (colorLoop) {
				index += ihue;
			}

			// brighten up, as the color palette itself often contains the 
			// light/dark dynamic range desired
			if (bri > 127) {
				bri = 255;
			}
			else {
				bri = dim8_raw(bri * 2);
			}

			CRGB color = ColorFromPalette(currentPalette, index, bri);
			leds[XYFront(i, j)] = color;
			leds[XYBack(i, j)] = color;
		}
	}

	ihue += 1;
}


//========= Rising Fire ===========
void fireRising( uint8_t colorIndex)
{
	for( int x = 0; x < LED_WIDTH; x++) {
		for ( int y = len[x]; y >= 0; y--)
		{
			leds[XY(x,y)] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
			colorIndex += 3;
		}
	}
}

//======== Sparkle ===============
void sparkle(uint8_t density, uint8_t colorIndex )
{
	for (int i = 0; i < density; i++)
	{
		int index = random8(NUM_LEDS);
		leds[index] = ColorFromPalette(currentPalette, colorIndex, BRIGHTNESS, NOBLEND);
		colorIndex += 10;
	}
}

//======== Rotating Vertical Bars ===============
void drawBar(uint8_t xPos, uint8_t colorIndex, uint8_t brightness)
{
	if (xPos >= 0)
	{
		for (int i = 0; i<13; i++)
		{
			if (XY(xPos, i) != -1)
			{
				leds[XY(xPos, i)] = ColorFromPalette(currentPalette, colorIndex, brightness, LINEARBLEND);
				colorIndex += 1;
			}
		}
	}
}

//============Rainbow=============
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

////============RotatingPalette=============
//void RotatingPalette( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
//{
//	byte lineStartHue = startHue8;
//	for( byte y = 0; y < LED_HEIGHT; y++) {
//		lineStartHue += yHueDelta8;
//		byte pixelHue = lineStartHue;      
//		for( byte x = 0; x < LED_WIDTH; x++) {
//			pixelHue += xHueDelta8;
//			leds[ XY(x, y)] = 
//				ColorFromPalette(currentPalette,lineStartHue/16,BRIGHTNESS,currentBlending );//CHSV( pixelHue, 255, 255);
//		}
//	}
//}

//======PALETTE example======

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
	for( int i = 0; i < NUM_LEDS; i++) {
		leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
		colorIndex += 3;
	}
}


//===========Twinkle===============
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
	// using Arduino 'bitRead' function; expanded code below
	return bitRead( directionFlags[index], bitNum);
	// uint8_t  andMask = 1 << bitNum;
	// return (directionFlags[index] & andMask) != 0;
}

void setPixelDirection( uint16_t i, bool dir) {
	uint16_t index = i / 8;
	uint8_t  bitNum = i & 0x07;
	// using Arduino 'bitWrite' function; expanded code below
	bitWrite( directionFlags[index], bitNum, dir);
	//  uint8_t  orMask = 1 << bitNum;
	//  uint8_t andMask = 255 - orMask;
	//  uint8_t value = directionFlags[index] & andMask;
	//  if( dir ) {
	//    value += orMask;
	//  }
	//  directionFlags[index] = value;
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



// Plasma (RGBShades)
// RGB Plasma
byte offset = 0; // counter for radial color wave motion
int plasVector = 0; // counter for orbiting plasma center
void plasma() {
	// Calculate current center of plasma pattern (can be offscreen)
	int xOffset = cos8(plasVector / 256);
	int yOffset = sin8(plasVector / 256);

	// Draw one frame of the animation into the LED array
	for (int x = 0; x < LED_WIDTH_HALF; x++) {
		for (int y = 0; y < LED_HEIGHT; y++) {
			byte color = sin8(sqrt(pow(((float)x - 7.5) * test + xOffset - 127, 2) + pow(((float)y - 2) * test + yOffset - 127, 2)) + offset);
			leds[XYFront(x, y)] = CHSV(color, 255, 255);
			leds[XYBack(x, y)] = CHSV(color, 255, 255);
		}
	}

	offset++; // wraps at 255 for sin8
	plasVector += 16; // using an int for slower orbit (wraps at 65536)

}