
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
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

Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(40, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel *stripPt3 = &strip3;

Adafruit_NeoMatrix matrix1 = Adafruit_NeoMatrix(8, 5, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix *matrixPt1 = &matrix1;
  
uint8_t strip1Speed = 100;
uint8_t strip2Speed = 1000;
uint8_t strip3Speed = 1000;
uint8_t strip4Speed = 1000;

const uint8_t DEFAULT_BRIGHTNESS = 30;
const uint8_t CYCLE_DELAY = 30;

// Colors
const uint32_t GOLD = matrix1.Color(162, 144, 96);
const uint32_t MUSTARD = matrix1.Color(44, 189, 12);
const uint32_t SKY_BLUE = matrix1.Color(0, 178, 239);
const uint32_t DEEP_BLUE = matrix1.Color(0, 87, 184);
const uint32_t DARK_CYAN = matrix1.Color(0, 140, 149);
const uint32_t KERMIT = matrix1.Color(67, 176, 42);
const uint32_t VIVID_ORANGE = matrix1.Color(227, 82, 5);

class NeoPixelController {
  
  protected: uint16_t cycleSize;
  protected: uint16_t stepOn = 0;
  protected: uint16_t ledOn = 0;
  protected: uint8_t colorOn = 0;
  protected: uint16_t cyclePoint = 0;
  protected: uint16_t runSpeed;
  protected: LinkedList<uint32_t> colors = LinkedList<uint32_t>();

  public: setCycleSize(uint16_t inCycleSize) {
    cycleSize = inCycleSize;
  }

  public: uint16_t getCycleSize() {
    return cycleSize;
  }
  
  public: setRunSpeed(uint16_t inRunSpeed) {
    runSpeed = inRunSpeed;
  }

  public: uint16_t getRunSpeed() {
    return runSpeed;
  }

  public: addColor(uint32_t  inColor) {
    colors.add(inColor);
  }

  public: uint32_t getColor(int num) {
    return colors.get(num);
  }

  public: clearColor() {
    colors.clear();
  }

  //constructers

  public: NeoPixelController (uint16_t inCycleSize, uint16_t inRunSpeed) {
    setCycleSize(inCycleSize);
    setRunSpeed(inRunSpeed);
  }

  public: NeoPixelController () {
    setCycleSize(100);
    setRunSpeed(100);
  }

  //logic

  public: virtual cycle() {
    cyclePoint += cycleSize;

    if(cyclePoint >= runSpeed){ //
      cycleStep();
      cyclePoint = 0;
    }
  }

  public: virtual cycleColor() {
    colorOn++;
    if(colorOn >= colors.size()) {
      colorOn = 0;
    }
  }

  public: virtual cycleStep() {

  }
  
  public: virtual setPixel(int pixelNumber, uint32_t  inColor) {

  }

  public: virtual setBrightness(uint8_t inBrightness) {

  }
  
};

class NeoPixelStripController: public NeoPixelController {

  protected: Adafruit_NeoPixel * strip;

    //Base getters/setters

  public: setStrip(Adafruit_NeoPixel * inStrip) {
    strip = inStrip;
  }

  public: Adafruit_NeoPixel * getStrip() {
    return strip;
  }

  //constructers

  public: NeoPixelStripController (Adafruit_NeoPixel * inStrip, uint16_t inCycleSize, uint16_t inRunSpeed) : NeoPixelController (inCycleSize, inRunSpeed) {
    setStrip(inStrip);
  }
  
  //logic

  public: virtual cycleStep() {
    for(int i = 0; i < strip->numPixels(); i++) {
      setPixel(i, colors.get(colorOn));
    }
    strip->show();
    cycleColor();
  }
  
  public: virtual setPixel(int pixelNumber, uint32_t  inColor) {
    strip->setPixelColor(pixelNumber, inColor);
  }

  public: setBrightness(uint8_t inBrightness) {
    strip->setBrightness(inBrightness);
  }
  
};

class NeoPixelMatrixController: public NeoPixelController {

  //Supported GFX Implementations
  public: static const uint8_t NEO_MATRIX = 0;

  protected: Adafruit_GFX * matrix;
  protected: uint8_t gfxImpl = NEO_MATRIX;

  //Base getters/setters

  public: setGfxImpl (uint8_t value) {
    gfxImpl = value;
  }

  public: uint8_t getGfxImpl() {
    return gfxImpl;
  }

  public: setMatrix (Adafruit_GFX * inMatrix) {
    matrix = inMatrix;
  }

  public: Adafruit_GFX * getMatrix() {
    return matrix;
  }

  //constructers

  public: NeoPixelMatrixController (Adafruit_GFX * inMatrix, uint16_t inCycleSize, uint16_t inRunSpeed) : NeoPixelController (inCycleSize, inRunSpeed) {
    setMatrix(inMatrix);
  }
  
  //logic

  public: virtual cycleStep() {
    matrix->fillScreen(colors.get(colorOn));
    show();
    cycleColor();
  }
  
  public: virtual setPixel(int pixelNumber, uint32_t  inColor) {
//    matrix->setPixelColor(pixelNumber, inColor);
  }

  public: setBrightness(uint8_t inBrightness) {  
    switch(gfxImpl) {
      case NEO_MATRIX:
        ((Adafruit_NeoMatrix*)matrix)->setBrightness(inBrightness);
        break;
      
    }
  }

  public: virtual show() {

    switch(gfxImpl) {
      case NEO_MATRIX:
        ((Adafruit_NeoMatrix*)matrix)->show();
        break;
      
    }
    
  }
  
};

class NeoPixelRectWiper: public NeoPixelMatrixController {

  protected: float ratio = 1;
  protected: uint8_t origX = 0;
  protected: uint8_t origY = 0;

  //Base getters/setters

  public: setRatio (float value) {
    ratio = value;
  }

  public: float getRatio() {
    return ratio;
  }

  public: setOrigX (uint8_t value) {
    origX = value;
  }

  public: uint8_t getOrigX() {
    return origX;
  }

  public: setOrigY (uint8_t value) {
    origY = value;
  }

  public: uint8_t getOrigY() {
    return origY;
  }

  //constructers
  
  public: NeoPixelRectWiper (Adafruit_GFX * inMatrix, uint16_t inCycleSize, uint16_t inRunSpeed) : NeoPixelMatrixController (inMatrix, inCycleSize, inRunSpeed) {
    setOrigX(inMatrix->width()/2);
    setOrigY(inMatrix->height()/2);
    setRatio(inMatrix->width()/inMatrix->height());
    ledOn = 0;
  }

  //logic

  public: virtual cycleStep() {
    if(ledOn > matrix->width() && ledOn > matrix->height()) {
      cycleColor();
      ledOn = 0;
    }
    matrix->fillRect(origX - ledOn, (origY - ledOn)/ratio, ledOn*2 - matrix->width()%2, (ledOn*2)/ratio - matrix->height()%2, colors.get(colorOn));
    show();
    ledOn++;
  }
  
};

class NeoPixelWiper: public NeoPixelStripController {

  //constructers

  public: NeoPixelWiper (Adafruit_NeoPixel * inStrip, uint16_t inCycleSize, uint16_t inRunSpeed) : NeoPixelStripController (inStrip, inCycleSize, inRunSpeed) {

  }
  
  //logic

  public: virtual cycleStep() {
    if(ledOn >= strip->numPixels()) {
      cycleColor();
      ledOn = 0;
    }
    setPixel(ledOn, colors.get(colorOn));
    strip->show();
    ledOn++;
  }

};

class NeoPixelAlternator: public NeoPixelStripController {

  //constructers

  public: NeoPixelAlternator (Adafruit_NeoPixel * inStrip, uint16_t inCycleSize, uint16_t inRunSpeed) : NeoPixelStripController (inStrip, inCycleSize, inRunSpeed) {

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

    strip->show();

  }
  
};

class NeoPixelRandomizer: public NeoPixelStripController {

  uint8_t pixelPerCycle = 1;

  public: setPixelPerCycle(uint8_t value) {
    pixelPerCycle = value;
  }

  public: uint8_t getPixelPerCycle() {
    return runSpeed;
  }

  //constructers

  public: NeoPixelRandomizer (Adafruit_NeoPixel * inStrip, uint16_t inCycleSize, uint16_t inRunSpeed) : NeoPixelStripController (inStrip, inCycleSize, inRunSpeed) {

  }
  
  //logic

  public: virtual cycleStep() {

    ledOn = random(strip->numPixels());
    colorOn = random(colors.size());

    for(int i = 0; i < pixelPerCycle  ; i++) {
      ledOn = random(strip->numPixels());
      setPixel(ledOn, colors.get(colorOn));
    }

    strip->show();

  }
  
};

NeoPixelStripController *control1;
NeoPixelWiper *control2;
NeoPixelController *control3;

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

  matrix1.begin();
  matrix1.setTextWrap(false);
  matrix1.setBrightness(40);

  //control3 = new NeoPixelStripController(stripPt3,CYCLE_DELAY,100);
  //control3 = new NeoPixelWiper(stripPt3,CYCLE_DELAY,100);
  //control3 = new NeoPixelAlternator(stripPt3,CYCLE_DELAY,100);
  //control3 = new NeoPixelRandomizer(stripPt3,CYCLE_DELAY,100);
  //control3 = new NeoPixelMatrixController(matrixPt1,CYCLE_DELAY,1000);
  control3 = new NeoPixelRectWiper(matrixPt1,CYCLE_DELAY,1000);
  ((NeoPixelRectWiper*)control3)->setOrigX(4);
  ((NeoPixelRectWiper*)control3)->setOrigY(3);

  //control3->addColor(GOLD);
  control3->addColor(MUSTARD);
  control3->addColor(SKY_BLUE);
  //control3->addColor(DEEP_BLUE);
 // control3->addColor(DARK_CYAN);
  control3->addColor(KERMIT);
  control3->addColor(VIVID_ORANGE);
  //control3->setPixelPerCycle(3);
  //control3->addColor(matrix1.Color(255, 0, 0)); //RED
  //control3->addColor(matrix1.Color(0, 255, 0)); //GREEN
  //control3->addColor(matrix1.Color(0, 0, 255)); //BLUE
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
