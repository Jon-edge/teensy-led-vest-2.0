TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myFirePalette_p PROGMEM;
extern const TProgmemPalette16 myRedBluePalette_p PROGMEM;
extern const TProgmemPalette16 myRastaPalette_p PROGMEM;
extern const TProgmemPalette16 myCostumePalette_p PROGMEM;

void SetupBlackAndWhiteStripedPalette()
{
	// 'black out' all 16 palette entries...
	fill_solid( currentPalette, 16, CRGB::Black);
	// and set every fourth one to white.
	currentPalette[0] = CRGB::White;
	currentPalette[4] = CRGB::White;
	currentPalette[8] = CRGB::White;
	currentPalette[12] = CRGB::White;
}

void SetupHeavenPalette()
{
	// 'black out' all 16 palette entries...
	fill_solid( currentPalette, 16, CRGB::Black);

	currentPalette[0] = CRGB::White;
	currentPalette[1] = CRGB::Grey;
	currentPalette[3] = CRGB::Grey;
	currentPalette[5] = (CRGB)CHSV(HUE_YELLOW, 100, 255);
	currentPalette[7] = CRGB::LightCyan;
	currentPalette[8] = CRGB::Grey;
	currentPalette[10] = (CRGB)CHSV(HUE_YELLOW, 150, 255);
	currentPalette[12] = CRGB::Grey;
	currentPalette[13] =  (CRGB)CHSV(HUE_BLUE, 50, 255);
	currentPalette[15] = CRGB::White;
}

void SetupPurpleAndGreenPalette()
{
	CRGB purple = CHSV( HUE_PURPLE, 255, 255);
	CRGB green  = CHSV( HUE_GREEN, 255, 255);
	CRGB black  = CRGB::Black;

	currentPalette = CRGBPalette16( 
		green,  green,  purple,  black,
		purple, purple, green,  black,
		green,  green,  purple,  black,
		purple, purple, green,  black );
}

const TProgmemPalette16 myFirePalette_p PROGMEM =
{
	CRGB::Red,
	CRGB::Red,
	CRGB::Black,
	CRGB::Red,
	CRGB::Red,
	CRGB::Orange,
	CRGB::Red,
	CRGB::Black,
	CRGB::Black,
	CRGB::Red,
	CRGB::Black,
	CRGB::Black,
	CRGB::Black,
	CRGB::Red,
	CRGB::Black,
	CRGB::Black
};

const TProgmemPalette16 myRedBluePalette_p PROGMEM =
{
	CRGB::Red,
	CRGB::Red, 
	CRGB::Red,
	CRGB::Black,
	CRGB::Blue,
	CRGB::Blue, 
	CRGB::Blue,
	CRGB::Black,
	CRGB::Red,
	CRGB::Red,
	CRGB::Red,
	CRGB::Black,
	CRGB::Blue,
	CRGB::Blue,
	CRGB::Blue,
	CRGB::Black
};

const TProgmemPalette16 myRastaPalette_p PROGMEM =
{
	CRGB::Red,
	CRGB::Red, 
	CRGB::Red,
	CRGB::Black,
	CRGB::Yellow,
	CRGB::Yellow, 
	CRGB::Yellow,
	CRGB::Black,
	CRGB::Green,
	CRGB::Green,
	CRGB::Green,
	CRGB::Black,
	CRGB::Red,
	CRGB::Yellow,
	CRGB::Green,
	CRGB::Black
};

const TProgmemPalette16 myCostumePalette_p PROGMEM =
{
	CRGB::Red,
	CRGB::Orange,
	CRGB::Black,
	CRGB::Orange,
	CRGB::Red,
	CRGB::Orange,
	CRGB::Yellow,
	CRGB::Black,
	CRGB::Orange,
	CRGB::Red,
	CRGB::Orange,
	CRGB::Black,
	CRGB::Black,
	CRGB::Red,
	CRGB::Red,
	CRGB::Orange
};

void SetupKaskadePalette()
{
	CRGB purple = CRGB::DeepPink;
	CRGB pink   = CHSV( HUE_GREEN, 255, 255);
	CRGB aqua   = CHSV( HUE_AQUA, 255, 255);
	CRGB blue   = CHSV( HUE_BLUE, 255, 255);
	CRGB black  = CRGB::Black;

	currentPalette = CRGBPalette16( 
		purple,  black,  aqua,  black,
		pink, black, blue,  black,
		aqua,  blue,  purple,  pink,
		black, purple, aqua,  black );
}

void SetupPurplePalette()
{
	CRGB purple = CHSV( HUE_PURPLE, 255, 255);
	CRGB pink   = CHSV( HUE_PINK, 255, 255);
	CRGB blue   = CRGB( 100, 0, 255);
	CRGB red    = CRGB::Red;
	CRGB black  = CRGB::Black;

	currentPalette = CRGBPalette16( 
		purple,  blue,  purple,  red,
		pink, red, purple,  black,
		pink,  purple,  purple,  pink,
		blue, purple, red,  black );
}

void SetupPastelPalette()
{
	CRGB aqua = CHSV( HUE_AQUA, 255, 255);
	CRGB pink   = CHSV( HUE_PINK, 150, 255);
	CRGB yellow   = CHSV( HUE_YELLOW, 255, 255);
	CRGB green   = CHSV( HUE_GREEN, 128, 255);
	CRGB black  = CRGB::Black;

	currentPalette = CRGBPalette16( 
		aqua,  pink,  yellow,  green,
		black, pink, pink,  black,
		yellow,  yellow,  black,  green,
		green, black, aqua,  aqua );
}

void selectPalette(uint8_t index)
{
	switch(index)
    {
        case 0: { currentPalette = myRastaPalette_p; currentBlending = LINEARBLEND; } break;
		case 1: { SetupPurpleAndGreenPalette(); currentBlending = LINEARBLEND; }break;
		case 2: { currentPalette = myRedBluePalette_p; currentBlending = LINEARBLEND; }break;
		case 3: { SetupBlackAndWhiteStripedPalette(); currentBlending = LINEARBLEND; }break;
		case 4: { currentPalette = myFirePalette_p; currentBlending = LINEARBLEND; }break;
		case 5: { SetupPurplePalette(); currentBlending = LINEARBLEND; }break;
		case 6: { currentPalette = CloudColors_p; currentBlending = LINEARBLEND; }break;
		case 7: { SetupPastelPalette(); currentBlending = LINEARBLEND; }break;
        case 8: { currentPalette = RainbowColors_p; currentBlending = LINEARBLEND; }break;
        case 9: { currentPalette = LavaColors_p; currentBlending = LINEARBLEND; }break;
        case 10: { SetupKaskadePalette(); currentBlending = LINEARBLEND; }break;
        case 11: { currentPalette = ForestColors_p; currentBlending = LINEARBLEND; }break;
        default: {currentPaletteIndex = 0; 
				  Serial.println("Invalid palette request, setting to 0");
				  selectPalette(currentPaletteIndex);}break;
	}                          
}

void ChangePalettePeriodically(bool isRandom = false)
{
	if (isAutoPaletteCycle)
	{
		if (isRandom)
		{
			currentPaletteIndex = random8(0,12);
		}
		else
		{
			currentPaletteIndex++;
			Serial.print("Auto Changing Palette: "); 
			Serial.println(currentPaletteIndex);
		}
	}
}

