
#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 193
#define LED_PIN 6            // Data pin for LED strip
#define LED_TYPE WS2812B     // Specify the type of LED strip
#define COLOR_ORDER GRB      // Color order of the LED strip
#define INITIAL_SPEED 20     // Initial animation delay in milliseconds

// max dimensions
extern const uint8_t LED_WIDTH;
extern const uint8_t LED_WIDTH_HALF;
extern const uint8_t LED_HEIGHT;

//   SubStrip index       0  1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
//   Direction            u  d   u   d   u   d   u   d   u   d   u   d   u  *u   d   u   d
extern uint8_t len[17];
extern uint8_t preDeadZone[17];

// Control
extern uint8_t MODE;
extern uint8_t BRIGHTNESS;
extern uint8_t brightness[9];
extern uint8_t brightnessIndex;
extern uint8_t textIndex;

extern CRGB leds_plus_safety_pixel[194];
extern CRGB* leds;

extern int XY(uint8_t x, uint8_t y);
extern int XYBack(uint8_t x, uint8_t y);
extern int XYFront(uint8_t x, uint8_t y);

extern void fillLeds(CRGB color);
extern void clearLeds();
extern void fadeAll(byte fadeIncr);