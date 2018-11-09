// This sketch was written to work with a V3 Flora, and 6 NeoPixels
// Some code snippets were taken from the examples provided here: https://github.com/adafruit/Adafruit_NeoPixel/tree/master/examples

// https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

#define ON_BOARD_PIN 8
#define STRIP_PIN 12
#define BUTTON_PIN 1
#define DEBUG 0

// Flora V3 has an onboard NeoPixel that I'll need to disable, because it won't be visiable
Adafruit_NeoPixel onboard = Adafruit_NeoPixel(1, ON_BOARD_PIN, NEO_GRB + NEO_KHZ800);

// Two Pixels per Color = 6 pixels.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, STRIP_PIN, NEO_GRB + NEO_KHZ800);

// These Vars make the later code easier to read-
uint32_t colorRed = Adafruit_NeoPixel::Color(255, 0, 0);
uint32_t colorYellow = Adafruit_NeoPixel::Color(251, 185, 83);
uint32_t colorGreen = Adafruit_NeoPixel::Color(0, 255, 0);
uint32_t colorOff = Adafruit_NeoPixel::Color(0, 0, 0);


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 1000;    // the debounce time; increase if the output flickers
bool lightstate, lastButtonState = false;


void setup() {
  // Mode switch button initialization 
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Turn off onboard pixel
  onboard.begin();
  onboard.setBrightness(0);
  onboard.show();

  // Set the R-Y-G pixels to max brightness
  strip.begin();
  strip.setBrightness(255);
  strip.show();

  // Setup button press ISR (WTF did I use interrupts?!?) 
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), togglestate, LOW);
}


void loop() {

  if (DEBUG) { // Who knew using conductive thread was so hard?!
    strip.setPixelColor(0, colorGreen);
    strip.setPixelColor(1, colorGreen);
    strip.setPixelColor(2, colorGreen);
    strip.setPixelColor(3, colorGreen);
    strip.setPixelColor(4, colorGreen);
    strip.setPixelColor(5, colorGreen);
    strip.show();
  }

// Default lightstate is false, which means Traffic Light Mode
  if (lightstate) { 
    // lightstate = true (Button Pushed)
    // Disco Mode!
    DoLightShow();
  } else {
     // lightstate = false (Button not pushed)
    TrafficCycle();
  }
}

//ISR Code-
void togglestate() {
  //  // read the state of the switch into a local variable:
  //  //int reading = digitalRead(buttonPin);
  // button pushed. Record the time
  unsigned long CurrentPushTime = millis();
  bool StateAtISRCall = lightstate;
  //Serial.print("State: "); Serial.println(StateAtISRCall);
  //Serial.print("LastButtonState: "); Serial.println(lastButtonState);
  // Serial.print("LastDebounceTime("); Serial.print(lastDebounceTime); Serial.print(") + debounceDelay("); Serial.print(debounceDelay); Serial.print(") > "); Serial.println(CurrentPushTime);
  if  ((lastDebounceTime + debounceDelay) > CurrentPushTime) {
    // still debouncing. update lastDebuncetime to currentpushtime
    lastDebounceTime = CurrentPushTime;
    //    Serial.println("Still debouncing");
  } else {
    //past debouncing guard
    //flip state
    lightstate = !lightstate;
    lastDebounceTime = millis();
    Serial.println("State changed");
  }

}


//Disco Mode function
void DoLightShow() {
  // Most of this was cribbed from the Adafruit NeoPixel Library examples
  
  // Save the current lightstate
  bool EnteredState = lightstate;
  
  uint8_t wait = 50;
   
  //affected = true;
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    if (lightstate != EnteredState) {
      return;
    }
    for (int q = 0; q < 3; q++) {
      if (lightstate != EnteredState) { return; } //If ISR is triggered, then jump out of this function-
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        if (lightstate != EnteredState) { return; } //If ISR is triggered, then jump out of this function-
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      if (lightstate != EnteredState) { return; } //If ISR is triggered, then jump out of this function-
      strip.show();
      myDelay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        if (lightstate != EnteredState) { return; } //If ISR is triggered, then jump out of this function-
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    } //End for (int q = 0; q < 3; q++)
    if (lightstate != EnteredState) { return; } //If ISR is triggered, then jump out of this function-
  } //End for (int j = 0; j < 256; j++) {
} //End void DoLightShow()


// Normal Mode Function
void TrafficCycle() {
  onboard.show();
  // affected = true;
  //Set Off - Off - G
  strip.setPixelColor(0, colorOff); //Top1
  strip.setPixelColor(1, colorOff); //Top2
  strip.setPixelColor(2, colorOff); //Middle1
  strip.setPixelColor(3, colorOff); //Middle2
  strip.setPixelColor(4, colorGreen); //Bottom1
  strip.setPixelColor(5, colorGreen); //Bottom2 
  strip.show();  
  // Pause 6 seconds
  myDelay(6000);
  
  //Set Off - Y - Off
  strip.setPixelColor(0, colorOff);
  strip.setPixelColor(1, colorOff);
  strip.setPixelColor(2, colorYellow);
  strip.setPixelColor(3, colorYellow);
  strip.setPixelColor(4, colorOff);
  strip.setPixelColor(5, colorOff);
  strip.show();
    //Pause 2 seconds
  myDelay(2000);
  
  // Set R - Off - Off
  strip.setPixelColor(0, colorRed);
  strip.setPixelColor(1, colorRed);
  strip.setPixelColor(2, colorOff);
  strip.setPixelColor(3, colorOff);
  strip.setPixelColor(4, colorOff);
  strip.setPixelColor(5, colorOff);
  strip.show();
  //Pause 5 seconds
  myDelay(5000);

}

// This delay allows the ISR to break it, vs normal delay()
void myDelay(int ms)
{
  uint32_t starttime = micros(); //in microseconds
  while (ms > 0 && !lightstate)
  {
    if ( (micros() - starttime) >= 1000)  {
      //Increment starttime by 1 millisecond
      starttime += 1000;
      //Decrement milliseconds left to count
      ms--;
    }
  }
}


// Helper function for Disco Mode. Taken from NeoPixel example
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
