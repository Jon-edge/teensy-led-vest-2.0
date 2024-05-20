#include <FastLED.h>

extern CRGBPalette16 currentPalette;
extern uint8_t currentPaletteIndex;

extern TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;

extern const TProgmemPalette16 myFirePalette_p PROGMEM;
extern const TProgmemPalette16 myRedBluePalette_p PROGMEM;
extern const TProgmemPalette16 myRastaPalette_p PROGMEM;
extern const TProgmemPalette16 myCostumePalette_p PROGMEM;
extern const TProgmemPalette16 myFirePalette_p PROGMEM;
extern const TProgmemPalette16 myRedBluePalette_p PROGMEM;
extern const TProgmemPalette16 myRastaPalette_p PROGMEM;
extern const TProgmemPalette16 myCostumePalette_p PROGMEM;

void SetupAmericaPalette();
void SetupHeavenPalette();
void SetupPurpleAndGreenPalette();
void SetupKaskadePalette();
void SetupPurplePalette();
void SetupPastelPalette();

void selectPalette(uint8_t index);
void ChangePalettePeriodically(bool isRandom);

