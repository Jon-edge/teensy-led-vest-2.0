
#include <Arduino.h>
#include <FastLED.h>

void RotatingRainbow( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);

void circleWave(bool isPalette, uint8_t masterZoom, CRGB color = CRGB::Black, uint8_t yOrigin = 7);
void circleWavePalette();
void circleWaveRainbow();

void squareWave(bool isPalette, uint8_t masterZoom, float angle, CRGB color = CRGB::Black, uint8_t yOrigin = 7);

void squareWavePalette(float angle);

void squareWaveRainbow(float angle);

void FillLEDsFromPaletteColors( uint8_t colorIndex);

void colortwinkles(byte delay);

void sparkle(uint8_t density, uint8_t colorIndex );

void threeSine(float angle, 
               uint8_t offsetA, uint8_t offsetB, uint8_t offsetC,
               uint8_t inverseStretch,
               uint8_t inverseWidth);
void threeSineOg(bool isRotated, 
			   uint8_t offsetA, uint8_t offsetB, uint8_t offsetC,
			   uint8_t inverseStretch,
			   uint8_t inverseWidth);

void meteor(unsigned long currentMillis, int speed, int flickerMs, bool rainbow);

void drawFace(CRGB color, unsigned long fadeTimeMs);
void drawHeart(CRGB color, unsigned long fadeTimeMs);