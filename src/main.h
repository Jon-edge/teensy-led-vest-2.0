#include <FastLED.h>

#define IR_RECEIVE_PIN 3     // Pin connected to the IR receiver

// Define IR signal constants for color commands
#define RED_0    0xE619FF00
#define RED_1    0xE817FF00
#define ORANGE_0 0xBF40FF00
#define ORANGE_1 0xF50AFF00
#define YELLOW   0xE31CFF00
#define GREEN_0  0xE41BFF00
#define GREEN_1  0xED12FF00
#define TEAL_0   0xB34CFF00
#define TEAL_1   0xE11EFF00
#define TEAL_2   0xEB14FF00
#define BLUE_0   0xEE11FF00
#define BLUE_1   0xE916FF00
#define PURP_0   0xFB04FF00
#define PURP_1   0xF10EFF00
#define PINK     0xF00FFF00
#define WHITE    0xEA15FF00

// Additional button definitions for controls
#define BRT_UP   0xF609FF00
#define BRT_DN   0xE21DFF00
#define OFF      0xE01FFF00
#define ON       0xF20DFF00
#define FLASH    0xB24DFF00
#define STROBE   0xFF00FF00
#define FADE     0xE51AFF00
#define SMOOTH   0xF30CFF00

//oldschool rainbow

extern uint8_t gHue; // rotating "base color" used by many of the patterns

// Mode & Palette
extern bool isAutoModeCycle;
extern bool isAutoPaletteCycle;

//==============BUTTON==================
// constants won't change. They're used here to
// set pin numbers:
extern const uint8_t ledPin;      // the number of the onboard LED pin

void handleIrSignal(uint32_t code);