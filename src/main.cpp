#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.hpp>

#include "main.h"
#include "matrix.h"
#include "palettes.h"
#include "effects.h"

const uint8_t ledPin = LED_BUILTIN; // the number of the onboard LED pin

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// Mode & Palette
bool isAutoModeCycle = false;
bool isAutoPaletteCycle = false;

enum Mode
{
    MODE_CIRCLE_WAVE_RAINBOW,
    MODE_ROTATING_RAINBOW,
    MODE_CHASERS,
    MODE_TWINKLE,
    MODE_COUNT // This helps to know the count of modes
};
Mode MODE = MODE_CIRCLE_WAVE_RAINBOW;


// Variables to track IR command repeats
uint32_t lastIrCode = 0;
int repeatCount = 0;
unsigned long lastIrTime = 0;
const unsigned long repeatTimeout = 150; // Time in ms to wait for a repeat


// Animation specific:
volatile int animationSpeed = INITIAL_SPEED; // Animation delay in milliseconds, start with initial speed
static uint8_t chaserStartIndex = 0;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(10);

    Serial.begin(115200);
    // while (!Serial)
        ; // Wait for the serial port to connect.

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the IR receiver
    Serial.println(F("\n\nIR Receiver and FastLED are initialized."));

    selectPalette(11);
}

void loop()
{
    unsigned long ms = millis();
 // Handle IR signals
    if (IrReceiver.decode())
    {
        uint32_t code = IrReceiver.decodedIRData.decodedRawData;

        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
        {
            int repeatMs = ms - lastIrTime;
            Serial.println("Repeat of last command " + String(repeatMs));
            repeatCount++;
        }
        else
        {
            Serial.print(F("Received new IR signal: 0x"));
            Serial.println(code, HEX);

            lastIrCode = code;
            repeatCount = 0;
        }

        lastIrTime = ms;

        IrReceiver.resume(); // Prepare for receiving the next IR signal.
    }
    else
    {
        if (ms - lastIrTime > repeatTimeout) {
            // We're sure this is the press meaning
            handleIrSignal(lastIrCode, repeatCount);
            repeatCount = 0;
            lastIrTime = 0;
            lastIrCode = 0;
        }

        BRIGHTNESS = brightness[brightnessIndex];
        FastLED.setBrightness(BRIGHTNESS);

        switch (MODE)
        {
        case MODE_CIRCLE_WAVE_RAINBOW:
        {
            circleWaveRainbow();
        }
        break;
        case MODE_ROTATING_RAINBOW:
        {
            int32_t yHueDelta32 = ((int32_t)sin16(ms * (27 / 1)) * (350 / LED_WIDTH));
            int32_t xHueDelta32 = ((int32_t)sin16(ms * (39 / 1)) * (310 / LED_HEIGHT));
            RotatingRainbow(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
        }
        break;
        case MODE_CHASERS:
        {
            EVERY_N_MILLISECONDS(animationSpeed)
            {
                chaserStartIndex = chaserStartIndex + 1; /* motion speed */
            }
            FillLEDsFromPaletteColors(chaserStartIndex);
        }
        break;
        case MODE_TWINKLE:
        {
            colortwinkles(animationSpeed);
        }
        break;
        case MODE_COUNT:
        {
            MODE=MODE_CIRCLE_WAVE_RAINBOW;
        }break;
        }
    }

    FastLED.show();
}

void handleIrSignal(uint32_t code, int repeats)
{
    if (repeats >= 3)
    {
        // Handle the command differently if it's repeated 3 times
        switch (code)
        {
        case RED_0:
            // Add custom handling here for repeated RED_0
            Serial.println(F("Handling RED_0 repeated 3 times"));
            break;
        case RED_1:
            // Add custom handling here for repeated RED_1
            Serial.println(F("Handling RED_1 repeated 3 times"));
            break;
        case ORANGE_0:
            // Add custom handling here for repeated ORANGE_0
            Serial.println(F("Handling ORANGE_0 repeated 3 times"));
            break;
        case ORANGE_1:
            // Add custom handling here for repeated ORANGE_1
            Serial.println(F("Handling ORANGE_1 repeated 3 times"));
            break;
        case YELLOW:
            // Add custom handling here for repeated YELLOW
            Serial.println(F("Handling YELLOW repeated 3 times"));
            break;
        case GREEN_0:
            // Add custom handling here for repeated GREEN_0
            Serial.println(F("Handling GREEN_0 repeated 3 times"));
            break;
        case GREEN_1:
            // Add custom handling here for repeated GREEN_1
            Serial.println(F("Handling GREEN_1 repeated 3 times"));
            break;
        case TEAL_0:
            // Add custom handling here for repeated TEAL_0
            Serial.println(F("Handling TEAL_0 repeated 3 times"));
            break;
        case TEAL_1:
            // Add custom handling here for repeated TEAL_1
            Serial.println(F("Handling TEAL_1 repeated 3 times"));
            break;
        case TEAL_2:
            // Add custom handling here for repeated TEAL_2
            Serial.println(F("Handling TEAL_2 repeated 3 times"));
            break;
        case BLUE_0:
            // Add custom handling here for repeated BLUE_0
            Serial.println(F("Handling BLUE_0 repeated 3 times"));
            break;
        case BLUE_1:
            // Add custom handling here for repeated BLUE_1
            Serial.println(F("Handling BLUE_1 repeated 3 times"));
            break;
        case PURP_0:
            // Add custom handling here for repeated PURP_0
            Serial.println(F("Handling PURP_0 repeated 3 times"));
            break;
        case PURP_1:
            // Add custom handling here for repeated PURP_1
            Serial.println(F("Handling PURP_1 repeated 3 times"));
            break;
        case PINK:
            // Add custom handling here for repeated PINK
            Serial.println(F("Handling PINK repeated 3 times"));
            break;
        case WHITE:
            // Add custom handling here for repeated WHITE
            Serial.println(F("Handling WHITE repeated 3 times"));
            break;
        case BRT_UP:
            // Add custom handling here for repeated BRT_UP
            Serial.println(F("Handling BRT_UP repeated 3 times"));
            break;
        case BRT_DOWNN:
            // Add custom handling here for repeated BRT_DOWNN
            Serial.println(F("Handling BRT_DOWNN repeated 3 times"));
            break;
        case OFF:
            // Add custom handling here for repeated OFF
            Serial.println(F("Handling OFF repeated 3 times"));
            break;
        case ON:
            // Add custom handling here for repeated ON
            Serial.println(F("Handling ON repeated 3 times"));
            break;
        case FLASH:
            // Add custom handling here for repeated FLASH
            Serial.println(F("Handling FLASH repeated 3 times"));
            break;
        case STROBE:
            // Add custom handling here for repeated STROBE
            Serial.println(F("Handling STROBE repeated 3 times"));
            break;
        case FADE:
            // Add custom handling here for repeated FADE
            Serial.println(F("Handling FADE repeated 3 times"));
            break;
        case SMOOTH:
            // Add custom handling here for repeated SMOOTH
            Serial.println(F("Handling SMOOTH repeated 3 times"));
            break;
        }
    }
    else
    {
        // Handle each color command based on the received IR signal
        switch (code)
        {
        case RED_0:
            MODE = MODE_ROTATING_RAINBOW;
            break;
        case RED_1:
            MODE = MODE_CIRCLE_WAVE_RAINBOW;
            break;
        case ORANGE_0:
            MODE = MODE_CHASERS;
            break;
        case ORANGE_1:
            MODE = MODE_TWINKLE;
            break;
        case YELLOW:
            leds[4] = CRGB::Yellow;
            break;
        case GREEN_0:
            leds[5] = CRGB::Green;
            break;
        case GREEN_1:
            leds[6] = CRGB::Green;
            break;
        case TEAL_0:
            leds[7] = CRGB::Teal;
            break;
        case TEAL_1:
            leds[8] = CRGB::Teal;
            break;
        case TEAL_2:
            leds[9] = CRGB::Teal;
            break;
        case BLUE_0:
            leds[0] = CRGB::Blue;
            break;
        case BLUE_1:
            leds[1] = CRGB::Blue;
            break;
        case PURP_0:
            leds[2] = CRGB::Purple;
            break;
        case PURP_1:
            leds[3] = CRGB::Purple;
            break;
        case PINK:
            leds[4] = CRGB::Pink;
            break;
        case WHITE:
            leds[5] = CRGB::White;
            break;
        case BRT_UP:
            if (brightnessIndex < 9)
                brightnessIndex++;
            break;
        case BRT_DOWNN:
            if (brightnessIndex > 0)
                brightnessIndex--;
            break;
        case OFF:
            MODE = MODE_COUNT;
            break;
        case ON:
            MODE = MODE_CIRCLE_WAVE_RAINBOW;
            break;
        case FLASH:
            animationSpeed = 100;
            break;
        case STROBE:
            animationSpeed = 10;
            break;
        case FADE:
            animationSpeed = 1000;
            break;
        case SMOOTH:
            animationSpeed = 100;
            break;
        }
    }
}