#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.hpp>

#include "main.h"
#include "matrix.h"
#include "palettes.h"
#include "effects.h"

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// Mode & Palette
bool isAutoModeCycle = false;
bool isAutoPaletteCycle = false;

//==============BUTTON==================
// constants won't change. They're used here to
// set pin numbers:
const uint8_t modeButtonPin = 2;    // the number of the mode/brightness pushbutton pin
const uint8_t paletteButtonPin = 7;    // the number of the mode/brightness pushbutton pin
const uint8_t ledPin = 13;      // the number of the onboard LED pin

volatile int animationSpeed = INITIAL_SPEED;  // Animation delay in milliseconds, start with initial speed

void setup() {

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(10);

    Serial.begin(115200);
    while (!Serial);  // Wait for the serial port to connect.

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Start the IR receiver
    Serial.println(F("\n\nIR Receiver and FastLED are initialized."));

	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
}


void loop() {
    // Handle IR signals
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
            Serial.println(F("Repeat of last command"));
        } else {
            Serial.print(F("Received new IR signal: 0x"));
            Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

            handleIrSignal(IrReceiver.decodedIRData.decodedRawData);
        }
        IrReceiver.resume();  // Prepare for receiving the next IR signal.
    } else {
        BRIGHTNESS = brightness[brightnessIndex];
        FastLED.setBrightness( BRIGHTNESS );

        unsigned long ms = millis();

        switch (MODE)
        {
            case 0: {
                circleWaveRainbow();
            }
            break;
            case 1: {
                int32_t yHueDelta32 = ((int32_t)sin16(ms * (27 / 1)) * (350 / LED_WIDTH));
                int32_t xHueDelta32 = ((int32_t)sin16(ms * (39 / 1)) * (310 / LED_HEIGHT));
                RotatingRainbow(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
            }break;
        }
    }

    FastLED.show();
}


void handleIrSignal(uint32_t code) {
    // Handle each color command based on the received IR signal
    switch (IrReceiver.decodedIRData.decodedRawData) {
        case RED_0:   MODE=0; break;
        case RED_1:    MODE=1; break;
        case ORANGE_0: leds[2] = CRGB::Orange; break;
        case ORANGE_1: leds[3] = CRGB::Orange; break;
        case YELLOW:   leds[4] = CRGB::Yellow; break;
        case GREEN_0:  leds[5] = CRGB::Green; break;
        case GREEN_1:  leds[6] = CRGB::Green; break;
        case TEAL_0:   leds[7] = CRGB::Teal; break;
        case TEAL_1:   leds[8] = CRGB::Teal; break;
        case TEAL_2:   leds[9] = CRGB::Teal; break;
        case BLUE_0:   leds[0] = CRGB::Blue; break;
        case BLUE_1:   leds[1] = CRGB::Blue; break;
        case PURP_0:   leds[2] = CRGB::Purple; break;
        case PURP_1:   leds[3] = CRGB::Purple; break;
        case PINK:     leds[4] = CRGB::Pink; break;
        case WHITE:    leds[5] = CRGB::White; break;
    }
}