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
bool isOff = false;

bool isAutoModeCycle = false;
bool isAutoPaletteCycle = true;

bool isStrobeEnabled = false;
bool isStrobeHide = false;
int strobeInterval = 0;
int strobeShowDuration = 10;

// Alt Modes
bool isPaletteMode = false;

enum Mode
{
    MODE_CIRCLE_WAVE_RAINBOW,
    MODE_CIRCLE_WAVE,
    MODE_ROTATING_RAINBOW,
    MODE_SQUARE_WAVE_RAINBOW,
    MODE_SQUARE_WAVE,
    // MODE_METEOR_RAINBOW, // dead
    // MODE_METEOR, // dead
    MODE_SINE_SPIN,
    MODE_SINE_HOR,
    MODE_SINE_VERT,

    MODE_STATIC, // sparkle without clearing
    MODE_SPARKLE,
    MODE_TWINKLE,
    MODE_CHASERS,
    MODE_COUNT // This helps to know the count of modes
};
Mode MODE = MODE_SINE_SPIN;

// Variables to track IR command repeats
uint32_t lastIrCode = 0;
int repeatCount = 0;
unsigned long lastIrTime = 0;
const unsigned long repeatTimeout = 150; // Time in ms to wait for a repeat
bool newSignal = false;

// Animation specific:
volatile int animationSpeed = INITIAL_SPEED; // Animation delay in milliseconds, start with initial speed
float rotationAngle = 0.0;

// Overlays:
bool isDrawHeart = false;
bool isDrawFace = false;

// legacy
static uint8_t lgcyColorIndex = 0;
bool effectInit = false;

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

    selectPalette(1);
}

void loop()
{
    unsigned long ms = millis();
    
    // Handle IR signals
    if (IrReceiver.decode())
    {
        uint32_t code = IrReceiver.decodedIRData.decodedRawData;

        // Filter out invalid codes and handle new signals
        if (code != 0xFFFFFFFF && code != 0) {
            newSignal = true;
            
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
        } 
        else {
            //Serial.println("ignoring code");
        } 
        IrReceiver.resume(); // Prepare for receiving the next IR signal.
    }
    else
    {
        if (newSignal && ms - lastIrTime > repeatTimeout) {
            newSignal = false;
            // We're sure this is the press meaning
            handleIrSignal(lastIrCode); // , repeatCount); TODO: broken somehow
            repeatCount = 0;
            lastIrTime = 0;
            lastIrCode = 0;
        }

        renderMode(ms);
    }

    if (isStrobeHide || isOff) {
        FastLED.setBrightness(0);
    } else {
        BRIGHTNESS = brightness[brightnessIndex];
        FastLED.setBrightness(BRIGHTNESS);
    }
    FastLED.show();
}

unsigned long lastStrobe = 0;

void renderMode(unsigned long ms) {
    // Globals
    rotationAngle+= 0.25;
    if (rotationAngle >= 360) {
        rotationAngle = 0;
    }
    
    if (isAutoModeCycle)
	{
		EVERY_N_SECONDS(10)
		{
			MODE = static_cast<Mode>(random(MODE_COUNT));
			effectInit = false;
			Serial.print("(AUTOCYCLE) Entering mode ");
			Serial.println(MODE);
		}
	}

    if (isAutoPaletteCycle)
    {
        EVERY_N_SECONDS(5) {
            ChangePalettePeriodically(true);
            selectPalette(currentPaletteIndex);
        }
    }

    if (isStrobeEnabled) {
        if (isStrobeHide) {
            if (ms - lastStrobe >= strobeInterval * 10) {
                isStrobeHide = false;
                lastStrobe = ms; // Reset the timer for the show state
            }
        } else {
            if (ms - lastStrobe >= strobeShowDuration) {
                isStrobeHide = true;
                lastStrobe = ms; // Reset the timer for the hide state
            }
        }
    } else {
        isStrobeHide = false;
        isStrobeEnabled = false;
        strobeInterval = 0;
    }

    if (isStrobeHide) {
        FastLED.setBrightness(0);
    } else {
        BRIGHTNESS = brightness[brightnessIndex];
        FastLED.setBrightness(BRIGHTNESS);
    }

    switch (MODE)
    {
        case MODE_CIRCLE_WAVE_RAINBOW: circleWaveRainbow(); break;
        case MODE_CIRCLE_WAVE: circleWavePalette(); break;
        case MODE_ROTATING_RAINBOW: {
            int32_t yHueDelta32 = ((int32_t)sin16(ms * (27 / 1)) * (350 / LED_WIDTH));
            int32_t xHueDelta32 = ((int32_t)sin16(ms * (39 / 1)) * (310 / LED_HEIGHT));
            RotatingRainbow(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
        } break;
        case MODE_SQUARE_WAVE_RAINBOW: {
            squareWaveRainbow(rotationAngle);
        } break;
        case MODE_SQUARE_WAVE: {
            squareWavePalette(rotationAngle);
        } break;
        case MODE_CHASERS: {
            EVERY_N_MILLISECONDS(animationSpeed/2)
            {
                lgcyColorIndex = lgcyColorIndex + 1; /* motion speed */
            }
            FillLEDsFromPaletteColors(lgcyColorIndex);
        } break;
        case MODE_TWINKLE: colortwinkles(animationSpeed); break;
        case MODE_STATIC: {sparkle(20, lgcyColorIndex);} break;
        case MODE_SPARKLE: {clearLeds(); sparkle(20, lgcyColorIndex);} break;
        // case MODE_METEOR_RAINBOW: meteor(ms, 10, 10, true); break;


        // case MODE_METEOR: meteor(ms, 5, 5, 90, 10, 10, false); break;
        case MODE_SINE_SPIN: EVERY_N_MILLISECONDS(50) {clearLeds(); threeSine(rotationAngle, 8, 9, 10, 6.5, 7.5);} break;
        case MODE_SINE_HOR: EVERY_N_MILLISECONDS(50) {threeSineOg(false, 8, 9, 10, 6.5, 7.5);} break;
        case MODE_SINE_VERT: EVERY_N_MILLISECONDS(50) {threeSineOg(true, 8, 9, 10, 6.5, 7.5);} break;
        case MODE_COUNT: MODE=MODE_CIRCLE_WAVE_RAINBOW; break;
        default: 
            MODE=MODE_CIRCLE_WAVE_RAINBOW;
            break;
    }

    // Overlays:
    if (isDrawFace) {
        CRGB textColor = CRGB::White;
        if (MODE != MODE_CIRCLE_WAVE_RAINBOW && MODE != MODE_ROTATING_RAINBOW && MODE != MODE_SQUARE_WAVE_RAINBOW) {
            if ((currentPalette[0].r == currentPalette[0].g == currentPalette[0].b > 200) &&
                (currentPalette[4].r == currentPalette[4].g == currentPalette[4].b > 200) &&
                (currentPalette[8].r == currentPalette[8].g == currentPalette[8].b > 200) &&
                (currentPalette[12].r == currentPalette[12].g == currentPalette[12].b > 200)) {
                textColor = CRGB::Red;
            }
        }
        drawFace(textColor, 100);
    } else if (isDrawHeart) {
        CRGB heartColor = CRGB::White;
        if (MODE != MODE_CIRCLE_WAVE_RAINBOW && MODE != MODE_ROTATING_RAINBOW && MODE != MODE_SQUARE_WAVE_RAINBOW) {
            EVERY_N_MILLISECONDS(5)
            {
                switch (random8(0, 7))
                {
					case 0:
						heartColor = CRGB::White;
						break;
					case 1:
						heartColor = CRGB::Red;
						break;
					case 2:
						heartColor = CRGB::Pink;
						break;
					case 3:
						heartColor = CRGB::Red;
						break;
					case 4:
						heartColor = CRGB::Red;
						break;
					case 5:
						heartColor = CRGB::Pink;
						break;
					case 6:
						heartColor = CRGB::Red;
						break;
					case 7:
						heartColor = CRGB::Red;
						break;
                }
            }
        }
        drawHeart(heartColor, 200);
    }
}

void handleIrSignal(uint32_t code)
{
    if (isPaletteMode)
    {
        Serial.print(F("Repeated IR signal: 0x"));
        // Handle the command differently if it's repeated
        switch (code) {
            case RED_0:    selectPalette(0); break;
            case RED_1:    selectPalette(1); break;
            case ORANGE_0: selectPalette(2); break;
            case ORANGE_1: selectPalette(3);break;
            case YELLOW:   selectPalette(4); break;

            case GREEN_0:  selectPalette(5); break;
            case GREEN_1:  selectPalette(6); break;
            case TEAL_0:   selectPalette(7); break;
            case TEAL_1:   selectPalette(8); break;
            case TEAL_2:   selectPalette(9); break;

            case BLUE_0:   selectPalette(10); break;
            case BLUE_1:   selectPalette(11);break;
            case PURP_0:   selectPalette(12);break;
            case PURP_1:   selectPalette(13);break;
            case PINK:     selectPalette(14); break;

            case WHITE:    selectPalette(15); break;

            case BRT_UP: break;
            case BRT_DOWNN: break;
            
            case OFF: isPaletteMode = false; break;
            case ON: break;

            case FLASH: strobeShowDuration++; break;
            case STROBE: strobeShowDuration--; break;
            case FADE: {
                isOff = !isOff;
            } break;
            case SMOOTH: {

            }break;
        }
    }
    else
    {
        // Handle each color command based on the received IR signal
        switch (code) {
            case RED_0:    MODE = MODE_ROTATING_RAINBOW; break;
            case RED_1:    MODE = MODE_CIRCLE_WAVE_RAINBOW; break;
            case ORANGE_0: MODE = MODE_SQUARE_WAVE_RAINBOW; break;
            case ORANGE_1: MODE = MODE_CHASERS; break;
            case YELLOW:   MODE = MODE_SINE_SPIN; break;

            case GREEN_0:  MODE = MODE_STATIC; break;
            case GREEN_1:  MODE = MODE_CIRCLE_WAVE; break;
            case TEAL_0:   MODE = MODE_SQUARE_WAVE; break;
            case TEAL_1:   MODE = MODE_TWINKLE; break;
            case TEAL_2:   MODE = MODE_SINE_HOR; break;

            case BLUE_0:   MODE = MODE_SPARKLE; break;
            case BLUE_1:   break;
            case PURP_0:   {
                isDrawHeart = false;
                isDrawFace = !isDrawFace;

                if (isDrawFace) {
                    fillLeds(CRGB::Blue);
                    FastLED.show();
                }
                else {
                    fillLeds(CRGB::Red);
                    FastLED.show();
                }                    
                delay(500);
                clearLeds(); 

            }break;
            case PURP_1:{
                isDrawFace = false;
                isDrawHeart = !isDrawHeart;

                if (isDrawHeart) {
                    fillLeds(CRGB::Pink);
                    FastLED.show();
                }
                else {
                    fillLeds(CRGB::Red);
                    FastLED.show();
                }                    
                delay(500);
                clearLeds(); 
            }   break;
            case PINK:     MODE = MODE_SINE_VERT; break;

            case WHITE:    MODE = MODE_TWINKLE; break;

            case BRT_UP: if (brightnessIndex < 8) { brightnessIndex++; } break;
            case BRT_DOWNN: if (brightnessIndex > 0) { brightnessIndex--; } break;
            
            case OFF: isStrobeEnabled = false; isStrobeHide = false; break;
            case ON: isPaletteMode = true; break;

            case FLASH: {
                isStrobeEnabled = true; 
                strobeInterval++;
                Serial.println(String(isStrobeEnabled));
                Serial.println(String(strobeInterval));
             } break;
            case STROBE: {
                isStrobeEnabled = true;
                if (strobeInterval >= 2) strobeInterval--; 

                Serial.println(String(isStrobeEnabled));
                Serial.println(String(strobeInterval));
            }break;
            case FADE: {
                isAutoModeCycle = !isAutoModeCycle;
                if (isAutoModeCycle) {
                    fillLeds(CRGB::Green);
                    FastLED.show();
                }
                else {
                    fillLeds(CRGB::Red);
                    FastLED.show();
                }                    
                delay(500);
                clearLeds(); 
            } break;
            case SMOOTH: {
                isAutoPaletteCycle = !isAutoPaletteCycle; 
                if (isAutoPaletteCycle) {
                    fillLeds(CRGB::Teal);
                    FastLED.show();
                }
                else {
                    fillLeds(CRGB::Purple);
                    FastLED.show();
                }                    
                delay(500);
                clearLeds(); 
            }break;
        }
    }
}