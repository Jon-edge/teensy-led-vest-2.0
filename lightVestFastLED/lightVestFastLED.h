
#define LED_PIN  6
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS 193

//oldschool rainbow

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// Mode & Palette
bool effectInit = false;
bool isAutoModeCycle = false;
bool isAutoPaletteCycle = false;

CRGBPalette16 currentPalette;
uint8_t currentPaletteIndex = 0;

// max dimensions
const uint8_t LED_WIDTH = 17;
const uint8_t LED_WIDTH_HALF = 9;
const uint8_t LED_HEIGHT = 13;

//   SubStrip index       0  1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
//   Direction            u  d   u   d   u   d   u   d   u   d   u   d   u  *u   d   u   d
uint8_t len[17] =       { 7, 9, 10, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 10,  9,  7}; // length of each strip
uint8_t preDeadZone[17]={ 2, 3,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  1,  4};

// Control
uint8_t MODE = 12;
uint8_t BRIGHTNESS = 255;
uint8_t brightness[9] = {30, 50, 75, 100, 125, 150, 175, 200, 255};
uint8_t brightnessIndex = 6;
uint8_t textIndex = 0;

CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* leds( leds_plus_safety_pixel + 1);

//==============BUTTON==================
// constants won't change. They're used here to
// set pin numbers:
const uint8_t modeButtonPin = 2;    // the number of the mode/brightness pushbutton pin
const uint8_t paletteButtonPin = 7;    // the number of the mode/brightness pushbutton pin
const uint8_t ledPin = 13;      // the number of the onboard LED pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long startHold = 0;


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

int XY(uint8_t x, uint8_t y)
{
	//initial boundry check
	if ((x > LED_WIDTH) ||
		(y > LED_HEIGHT))
	{
		return -1;
	}

	uint8_t stripIndex = 0;  // overall position on strip as a whole
	uint8_t lastSegment = 0;  // Position on substrip after previous strips accounted for

	// Back portion
	if ((x > 3) && (x < 13))
	{
		if( x & 0x01) {
			// Odd rows run backwards
			lastSegment = (LED_HEIGHT - 1) - y;
		} else {
			// Even rows run forwards

			lastSegment = y;
		}
	}

	// Special handling for last segment for trapezoidal front panels
	else
	{
		// Diagonal deadzones
		if ( (((x==0)||(x==16)) && ((y<2)||(y>8))) ||
			(((x==1)||(x==15)) && ((y<1)||(y>9))) ||
			(((x==2)||(x==14)) && ((y<1)||(y>10))) ||
			(((x==3)||(x==13)) && (y>11)) )
		{
			return -1;
		}

		if(x & 0x01) {
			if (x<4)
			{
				// Odd rows run backwards on left
				lastSegment = (LED_HEIGHT - 1) - y;
			}
			else
			{
				// forwards on right
				lastSegment = y;
			}
		}
		else
		{
			if (x<4)
			{
				// Even rows run forwards on right
				lastSegment = y;
			}
			else
			{
				// backwards on left
				lastSegment = (LED_HEIGHT - 1) - y;
			}
		}

		// Subtract deadzone
		lastSegment -= preDeadZone[x];
	}

	// Collect index total as we move across the X axis
	if (x>0)
	{
		for (int i = 0; i < x; i++)
		{
			stripIndex += len[i];
		}
	}

	// Add final partial strip
	stripIndex += lastSegment;

	return stripIndex;
}

int XYBack(uint8_t x, uint8_t y)
{
	//initial boundry check
	if ((x > 8) || 
		(x < 0) ||
		(y > LED_HEIGHT) ||
		(y < 0))
	{
		return -1;
	}

	return XY(x+4, y);
}

int XYFront(uint8_t x, uint8_t y, bool omitDeadZone = false)
{
	if (omitDeadZone) // no deadzone in middle for empty space due to zipper
	{
		//initial boundry check
		if ((x > 7) || (y > LED_HEIGHT))
		{
			return -1;
		}

		if (x < 4)
		{
			return XY(x+13,y);
		}
		else
		{
			return XY(x-4, y);
		}
	}
	else // include deadzone in middle for empty space due to zipper
	{
		//initial boundry check
		if ((x > 8) || (x == 4) || (y > LED_HEIGHT))
		{
			return -1;
		}

		if (x < 4)
		{
			return XY(x+13,y);
		}
		else
		{
			return XY(x-5, y);
		}
	}
}

void fillLeds(CRGB color)
{
    for( int i = 0; i < NUM_LEDS; i++) {
		leds[i] = color;
	}
}

void clearLeds()
{
	fillLeds(CRGB::Black);
}

void fadeAll(byte fadeIncr) {
	for (byte i = 0; i < NUM_LEDS; i++) {
		leds[i] = leds[i].fadeToBlackBy(fadeIncr);
	}
}

void modeButtonLogic()
{
	static bool isInitialize = true;

	// Variables will change:
	static bool buttonState;             // the current reading from the input pin
	static bool lastButtonState = LOW;   // the previous reading from the input pin
    static bool isHoldPress = false;

	// read the state of the switch into a local variable:
	int reading = digitalRead(modeButtonPin);

	// check to see if you just pressed the button
	// (i.e. the input went from LOW to HIGH),  and you've waited
	// long enough since the last press to ignore any noise:

	// If the switch changed, due to noise or pressing:
	if (reading != lastButtonState) {
		// reset the debouncing timer
		lastDebounceTime = millis();
	}

	if (((millis() - lastDebounceTime) > debounceDelay) && (millis() > 100)) {
		// whatever the reading is at, it's been there for longer
		// than the debounce delay, so take it as the actual current state:

		// if the button state has changed:
		if (reading != buttonState) {
			buttonState = reading;

			// Button has been released
			if (buttonState == HIGH) {
				if (isInitialize)
				{
					isInitialize = false;
				}
				else
				{
					// Button wasn't released following a hold press,
                    // process as a normal mode change, disable auto mode cycle.
					if (isHoldPress != true)
					{
						isAutoModeCycle = false;
						effectInit = false;
						Serial.print("(Manual switch) Entering mode ");
						MODE++;
						Serial.println(MODE);
					}
					else
					{
						isHoldPress = false;
					}
				}
			}
			if (buttonState == LOW)
			{
				startHold = millis();
			}
		}
	}

    // Handle hold press logic
	if ((buttonState == LOW) && 
		(millis() - startHold > 500))
	{
		isHoldPress = true;
		startHold = millis();

		//Change auto mode cycle only on mode 0
		if (MODE == 0)
		{
			if (brightnessIndex < 9)
			{
				brightnessIndex += 1;
			}
			else
			{
				brightnessIndex = 0;
			}

			for (int i = 0; i < (brightnessIndex); i++)
			{
				leds[7 + i + 1] = CRGB::Red;
			}
			FastLED.show();
			delay(300);
		}
        else
		{
			CRGB indicatorColor;
            isAutoPaletteCycle = !isAutoPaletteCycle;
			// toggle auto palette cycle
			if (isAutoPaletteCycle)
			{
				indicatorColor = CRGB::Teal;
				Serial.println("Auto palette cycle ON");
			}
			else
			{
				indicatorColor = CRGB::Pink;
				Serial.println("Auto palette cycle OFF");
			}
			fillLeds(indicatorColor);
			FastLED.show();
			delay(300);
		}
        /*
		// auto mode cycle toggle
		else
		{
			CRGB indicatorColor;
			isAutoModeCycle = !isAutoModeCycle;
			// toggle auto palette cycle
			if (isAutoModeCycle)
			{
				indicatorColor = CRGB::Teal;
				Serial.println("Auto mode cycle ON");
			}
			else
			{
				indicatorColor = CRGB::Pink;
				Serial.println("Auto mode cycle OFF");
			}
			fillLeds(indicatorColor);
			FastLED.show();
			delay(300);
		}*/
	}

	// save the reading.  Next time through the loop,
	// it'll be the lastButtonState:
	lastButtonState = reading;
}

void paletteButtonLogic()
{
	static bool isInitialize2 = true;

	// Variables will change:
	static bool buttonState2;             // the current reading from the input pin
	static bool lastButtonState2 = LOW;   // the previous reading from the input pin
    static bool isHoldPress2 = false;

	// read the state of the switch into a local variable:
	int reading = digitalRead(paletteButtonPin);

	// check to see if you just pressed the button
	// (i.e. the input went from LOW to HIGH),  and you've waited
	// long enough since the last press to ignore any noise:

	// If the switch changed, due to noise or pressing:
	if (reading != lastButtonState2) {
		// reset the debouncing timer
		lastDebounceTime = millis();
	}

	if (((millis() - lastDebounceTime) > debounceDelay) && (millis() > 100)) {
		// whatever the reading is at, it's been there for longer
		// than the debounce delay, so take it as the actual current state:

		// if the button state has changed:
		if (reading != buttonState2) {
			buttonState2 = reading;

			// Button has been released
			if (buttonState2 == HIGH) {
				if (isInitialize2)
				{
					isInitialize2 = false;
				}
				else
				{
					// Button wasn't released following a hold press,
                    // process as a normal palette change.
					if (isHoldPress2 != true)
					{
						isAutoPaletteCycle = false;
						Serial.print("Palette "); 
						Serial.println(currentPaletteIndex);
						currentPaletteIndex++;
					}
					else
					{
						isHoldPress2 = false;
					}
				}
			}
			if (buttonState2 == LOW)
			{
				startHold = millis();
			}
		}
	}

    // Handle hold press logic
	if ((buttonState2 == LOW) && 
		(millis() - startHold > 600))
	{
		isHoldPress2 = true;
		startHold = millis();
		CRGB indicatorColor;

		if (MODE == 0)
		{
			// toggle auto palette cycle
			if (!isAutoPaletteCycle)
			{
				isAutoPaletteCycle = true;
				indicatorColor = CRGB::Green;
				Serial.println("Auto palette cycle ON");
			}
			else
			{
				isAutoPaletteCycle = false;
				indicatorColor = CRGB::Red;
				Serial.println("Auto palette cycle OFF");
			}
			fillLeds(indicatorColor);
			FastLED.show();
			delay(300);
		}
		else
		{
			textIndex += 1;
			Serial.print("textIndex = ");
			Serial.println(textIndex);
			for (int i = 0; i < (textIndex); i++)
			{
				leds[7 + i + 1] = CRGB::Blue;
			}
			FastLED.show();
			delay(300);
		}
	}

	// save the reading.  Next time through the loop,
	// it'll be the lastButtonState2:
	lastButtonState2 = reading;
}
