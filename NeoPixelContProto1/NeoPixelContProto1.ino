#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(2, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel *stripPt1 = &strip1;

Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(2, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel *stripPt2 = &strip2;

uint8_t strip1Speed = 100;
uint8_t strip2Speed = 1000;
uint8_t strip3Speed = 1000;
uint8_t strip4Speed = 1000;

struct NeoPixelWiper {

  public: static const uint8_t MAX_COLORS = 10;

  private: Adafruit_NeoPixel * strip;
  private: uint16_t cycleSize;
  private: uint16_t stepOn = 0;
  private: uint16_t ledOn = 0;
  private: uint8_t colorOn = 0;
  //private: uint8_t brightness = 0;
  private: uint16_t cyclePoint = 0;
  private: uint16_t runSpeed;
  private: uint8_t numColors = 0;
  private: uint32_t colors[MAX_COLORS];

  //Base getters/setters

  public: setStrip(Adafruit_NeoPixel * inStrip) {
    strip = inStrip;
  }

  public: Adafruit_NeoPixel * getStrip() {
    return strip;
  }

  public: setCycleSize(uint16_t inCycleSize) {
    cycleSize = inCycleSize;
  }

  public: uint16_t getCycleSize() {
    return cycleSize;
  }

  public: setBrightness(uint8_t inBrightness) {
    strip->setBrightness(inBrightness);
  }

  /*
  public: uint8_t getBrightness() {
    return brightness;
  }
  */
  
  public: setRunSpeed(uint16_t inRunSpeed) {
    runSpeed = inRunSpeed;
  }

  public: uint16_t getRunSpeed() {
    return runSpeed;
  }

  public: addColor(uint32_t  inColor) {
    colors[numColors++] = inColor;
  }

  public: uint32_t getColor(uint8_t num) {
    return colors[num];
  }

  public: clearColor() {
    numColors = 0;
  }

  //constructers

  public: NeoPixelWiper (Adafruit_NeoPixel * inStrip, uint16_t inCycleSize, uint16_t inRunSpeed) {
    setStrip(inStrip);
    setCycleSize(inCycleSize);
    setRunSpeed(inRunSpeed);
  }

  //logic

  public: cycle() {
    cyclePoint += cycleSize;

    if(cyclePoint >= runSpeed){ //
      cycleStep();
      cyclePoint = 0;
    }
  }

  public: cycleStep() {
    ledOn++;
    if(ledOn >= strip->numPixels()) {
      cycleColor();
      ledOn = 0;
    }
    strip->setPixelColor(ledOn, colors[colorOn]);
    strip->show();
  }

  public: cycleColor() {
    colorOn++;
    if(colorOn >= numColors) {
      colorOn = 0;
    }
  }
  
  
};

NeoPixelWiper *wiper1;
NeoPixelWiper *wiper2;

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  stripPt1->begin();
  stripPt1->setBrightness(30);
  stripPt1->show(); // Initialize all pixels to 'off'

  wiper1 = new NeoPixelWiper(stripPt1,30,1000);

  wiper1->addColor(strip1.Color(255, 0, 0)); //Red
  wiper1->addColor(strip1.Color(0, 255, 0)); //Green
  wiper1->addColor(strip1.Color(0, 0, 255)); //Blue
  wiper1->addColor(strip1.Color(255, 255, 255)); //White

  stripPt2->begin();
  stripPt2->setBrightness(30);
  stripPt2->show(); // Initialize all pixels to 'off'

  wiper2 = new NeoPixelWiper(stripPt2,30,100);

  wiper2->addColor(strip1.Color(255, 255, 0)); //Red
  wiper2->addColor(strip1.Color(0, 255, 255)); //Green
  wiper2->addColor(strip1.Color(255, 0, 255)); //Blue
}

void loop() {

  wiper1->cycle();
  //wiper1->cycleStep();
  wiper2->cycle();
  delay(30);
  
  
  // Some example procedures showing how to display to the pixels:
  //colorWipe(stripPt1, strip1.Color(255, 0, 0), strip1Speed); // Red
  //colorWipe(stripPt1, strip1.Color(0, 255, 0), strip1Speed); // Green
  //colorWipe(stripPt2, strip1.Color(0, 0, 255), strip2Speed); // Blue
  //colorWipe(stripPt2, strip1.Color(255, 255, 255), strip2Speed); // White RGBW
  // Send a theater pixel chase in...
  //theaterChase(strip1.Color(127, 127, 127), 50); // White
  //theaterChase(strip1.Color(127, 0, 0), 50); // Red
  //theaterChase(strip1.Color(0, 0, 127), 50); // Blue

  //rainbow(20);
  //rainbowCycle(20);
  //theaterChaseRainbow(50);
}

// Fill the dots one after the other with a color
void colorWipe(Adafruit_NeoPixel *inStrip, uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<inStrip->numPixels(); i++) {
    inStrip->setPixelColor(i, c);
    inStrip->show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip1.numPixels(); i++) {
      strip1.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip1.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip1.numPixels(); i++) {
      strip1.setPixelColor(i, Wheel(((i * 256 / strip1.numPixels()) + j) & 255));
    }
    strip1.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip1.numPixels(); i=i+3) {
        strip1.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip1.show();

      delay(wait);

      for (uint16_t i=0; i < strip1.numPixels(); i=i+3) {
        strip1.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip1.numPixels(); i=i+3) {
        strip1.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip1.show();

      delay(wait);

      for (uint16_t i=0; i < strip1.numPixels(); i=i+3) {
        strip1.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}