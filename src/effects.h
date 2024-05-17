
#include <Arduino.h>
#include <FastLED.h>

void RotatingRainbow( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
void circleWave(bool isPalette, uint8_t masterZoom, CRGB color = CRGB::Black, uint8_t yOrigin = 7);
void circleWaveRainbow();

void FillLEDsFromPaletteColors( uint8_t colorIndex);

void colortwinkles(byte delay);