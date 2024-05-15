#include <Arduino.h>
#include <IRremote.hpp>
#include <FastLED.h>

#define NUM_LEDS 10          // Number of LEDs in the strip
#define LED_PIN 6            // Data pin for LED strip
#define IR_RECEIVE_PIN 3     // Pin connected to the IR receiver
#define LED_TYPE WS2812B     // Specify the type of LED strip
#define COLOR_ORDER GRB      // Color order of the LED strip
#define INITIAL_SPEED 20    // Initial animation delay in milliseconds

CRGB leds[NUM_LEDS];
volatile int animationSpeed = INITIAL_SPEED;  // Animation delay in milliseconds, start with initial speed

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Wait for the serial port to connect.

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(50);

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Start the IR receiver
    Serial.println(F("IR Receiver and FastLED are initialized."));
    Serial.print(F("Initial Animation Speed: "));
    Serial.println(animationSpeed);
}

void loop() {
    // Handle IR signals
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
            Serial.println(F("Repeat of last command"));
        } else {
            Serial.print(F("Received new IR signal: 0x"));
            Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
            // Each new IR signal decreases the animation delay (increases speed)
            if (animationSpeed > 1) {  // Prevent the speed from becoming too fast to perceive
                animationSpeed -= 1;  // Decrease the delay, increasing the speed
            }
            Serial.print(F("Updated Animation Speed: "));
            Serial.println(animationSpeed);
        }
        IrReceiver.resume();  // Prepare for receiving the next IR signal.
    }

    // Rapidly running single light animation
    static int pos = 0;  // Position of the lit LED
    leds[pos] = CRGB::Red;  // Set the current LED to red
    FastLED.show();  // Update the strip
    leds[pos] = CRGB::Black;  // Turn off the LED after showing

    pos++;  // Move to the next LED
    if (pos >= NUM_LEDS) {
        pos = 0;  // Reset to the beginning of the strip
    }

    delay(animationSpeed);  // Control the speed of the animation based on the current setting
}
