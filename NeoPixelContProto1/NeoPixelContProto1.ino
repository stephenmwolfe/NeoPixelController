

#include <Adafruit_NeoPixel.h>
#include <LinkedList.h>

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

Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(4, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel *stripPt3 = &strip3;

uint8_t strip1Speed = 100;
uint8_t strip2Speed = 1000;
uint8_t strip3Speed = 1000;
uint8_t strip4Speed = 1000;

const uint8_t DEFAULT_BRIGHTNESS = 30;
const uint8_t CYCLE_DELAY = 30;

struct NeoPixelState {
  public: uint32_t color;
  public: uint8_t brightness;

  public: NeoPixelState (uint32_t inColor, uint8_t inBrightness) {
    color = inColor;
    brightness = inBrightness;
  }

  public: NeoPixelState () {
    color = 0;
    brightness = 0;
  }
  
};

class NeoPixelController {
  
  protected: Adafruit_NeoPixel * strip;
  protected: uint16_t cycleSize;
  protected: uint16_t stepOn = 0;
  protected: uint16_t ledOn = 0;
  protected: uint8_t colorOn = 0;
  protected: uint16_t cyclePoint = 0;
  protected: uint16_t runSpeed;
  protected: LinkedList<NeoPixelState> colors = LinkedList<NeoPixelState>();

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
  
  public: setRunSpeed(uint16_t inRunSpeed) {
    runSpeed = inRunSpeed;
  }

  public: uint16_t getRunSpeed() {
    return runSpeed;
  }

  public: addColor(uint32_t  inColor) {
    uint8_t brightness = strip->getBrightness();
    //NeoPixelState newState = new NeoPixelState(inColor,brightness);
    colors.add(NeoPixelState(inColor,brightness));
  }

  public: addColor(uint32_t  inColor, uint8_t inBrightness) {
    colors.add(NeoPixelState(inColor,inBrightness));
  }

  public: addColor(NeoPixelState  inColor) {
    colors.add(inColor);
  }

  public: NeoPixelState getColor(int num) {
    return colors.get(num);
  }

  public: clearColor() {
    colors.clear();
  }

  //constructers

  public: NeoPixelController (Adafruit_NeoPixel * inStrip, uint16_t inCycleSize, uint16_t inRunSpeed) {
    setStrip(inStrip);
    setCycleSize(inCycleSize);
    setRunSpeed(inRunSpeed);
  }

  //logic

  public: virtual cycle() {
    cyclePoint += cycleSize;

    if(cyclePoint >= runSpeed){ //
      cycleStep();
      cyclePoint = 0;
    }
  }

  public: virtual cycleStep() {
    for(int i = 0; i < strip->numPixels(); i++) {
      setPixel(i, colors.get(colorOn));
    }
    cycleColor();
  }

  public: virtual cycleColor() {
    colorOn++;
    if(colorOn >= colors.size()) {
      colorOn = 0;
    }
  }

  public: virtual setPixel(int pixelNumber, NeoPixelState state) {
    strip->setBrightness(state.brightness);
    strip->setPixelColor(pixelNumber, state.color);
    strip->show();
  }
  
  
};

class NeoPixelWiper: public NeoPixelController {

  //constructers

  public: NeoPixelWiper (Adafruit_NeoPixel * inStrip, uint16_t inCycleSize, uint16_t inRunSpeed) : NeoPixelController (inStrip, inCycleSize, inRunSpeed) {

  }
  
  //logic

  public: virtual cycleStep() {
    if(ledOn >= strip->numPixels()) {
      cycleColor();
      ledOn = 0;
    }
    setPixel(ledOn, colors.get(colorOn));
    ledOn++;
  }
  
};

class NeoPixelAlternator: public NeoPixelController {

  //constructers

  public: NeoPixelAlternator (Adafruit_NeoPixel * inStrip, uint16_t inCycleSize, uint16_t inRunSpeed) : NeoPixelController (inStrip, inCycleSize, inRunSpeed) {

  }
  
  //logic

  public: virtual cycleStep() {

    colorOn = ledOn++;

    for(int i = strip->numPixels() - 1; i >= 0 ; i--) {
      setPixel(i, colors.get(colorOn));
      cycleColor();
    }

    if(ledOn >= colors.size()) {
      ledOn = 0;
    }

    /*
    if(colors.size() <= strip->numPixels()) {
      if(ledOn >= strip->numPixels()) {
        ledOn = 0;
      }
      
      int adr = ledOn;
      
      for(int i = 0; i < strip->numPixels(); i++) {
        setPixel(adr, colors.get(colorOn));
        cycleColor();
        adr--;
        if(adr < 0){
          adr = strip->numPixels() - 1;
        }
      }
      colorOn = 0;
      ledOn++;
    }
    else {
      
    }
    */

  }
  
};

NeoPixelController *control1;
NeoPixelWiper *control2;
NeoPixelAlternator *control3;

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

  /*
  stripPt1->begin();
  stripPt1->setBrightness(DEFAULT_BRIGHTNESS);
  stripPt1->show(); // Initialize all pixels to 'off'

  control1 = new NeoPixelController(stripPt1,CYCLE_DELAY,1000);

  control1->addColor(strip1.Color(255, 0, 0)); //Red
  control1->addColor(strip1.Color(0, 255, 0)); //Green
  control1->addColor(strip1.Color(0, 0, 255)); //Blue
  control1->addColor(strip1.Color(255, 255, 255)); //White

  stripPt2->begin();
  stripPt2->setBrightness(DEFAULT_BRIGHTNESS);
  stripPt2->show(); // Initialize all pixels to 'off'

  wiper2 = new NeoPixelWiper(stripPt2,CYCLE_DELAY,200);

  wiper2->addColor(strip1.Color(255, 255, 0));
  wiper2->addColor(strip1.Color(0, 255, 255));
  wiper2->addColor(strip1.Color(255, 0, 255));
  */
  
  stripPt3->begin();
  stripPt3->setBrightness(DEFAULT_BRIGHTNESS);
  stripPt3->show(); // Initialize all pixels to 'off'

  control3 = new NeoPixelAlternator(stripPt3,CYCLE_DELAY,100);

  //control3->addColor(strip1.Color(255, 0, 0)); //Red
  //control3->addColor(strip1.Color(0, 255, 0)); //Green
  //control3->addColor(strip1.Color(0, 0, 255)); //Blue
  control3->addColor(strip1.Color(0, 0, 0)); //Black
  control3->addColor(strip1.Color(255, 255, 255),255); //White
  //control3->addColor(strip1.Color(255, 255, 0));
  //control3->addColor(strip1.Color(0, 255, 255));
  //control3->addColor(strip1.Color(255, 0, 255));
}

void loop() {

  //control1->cycle();
  //wiper1->cycleStep();
  //wiper2->cycle();
  control3->cycle();
  delay(CYCLE_DELAY);
  
  
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
