#include "FastLED.h"
 
// How many leds in your strip?
#define NUM_LEDS 96
 
// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 6
#define CLOCK_PIN 13
#define RELAY_PIN 5

#define COLOR_ORDER RBG
 
// Adalight sends a "Magic Word" (defined in /etc/boblight.conf) before sending the pixel data
uint8_t prefix[] = {'A', 'd', 'a'}, onsig[] = {'H', 'y', 'p'}, offsig[] = {'h', 'Y', 'P'}, sample, hi, lo, chk, i;
boolean relay_status = false;

// Baudrate, higher rate allows faster refresh rate and more LEDs (defined in /etc/boblight.conf)
#define serialRate 460800
 
// Define the array of leds
CRGB leds[NUM_LEDS];
 
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
      // Uncomment/edit one of the following lines for your leds arrangement.
      // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
      // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);
     
      // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);
 
      FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
       
        Serial.begin(serialRate);
        Serial.print("Ada\n"); // Send "Magic Word" string to host
}
 
void loop() {
  // wait for first byte of Magic Word
  for(i = 0; i < sizeof prefix; ++i) {
    waitLoop: while (!Serial.available()) ;;
    // Check next byte in Magic Word
    sample = Serial.read();
    if(prefix[i] == sample) continue;
    else if (offsig[0] == sample) {
      for(i = 1; i < sizeof offsig; ++i) {
        while (!Serial.available()) ;;
        if(offsig[i] == Serial.peek()) {
          Serial.read();
          continue;
        }
        else {// otherwise, start over
          i = 0;
          goto waitLoop;
        }
      }
      // Hi, Lo, Checksum for off signal
      while (!Serial.available()) ;;
      hi=Serial.read();
      while (!Serial.available()) ;;
      lo=Serial.read();
      while (!Serial.available()) ;;
      chk=Serial.read();
      
      if (chk == (hi ^ lo ^ 0x55)) {
        if (true == relay_status) {
          LEDS.showColor(CRGB(64, 0, 0));
          delay(100);
          LEDS.showColor(CRGB(0, 0, 0));
          delay(150);
          LEDS.showColor(CRGB(64, 0, 0));
          delay(100);
          LEDS.showColor(CRGB(0, 0, 0));
          relay_status = false;
          digitalWrite(RELAY_PIN, LOW);
        }
      }
      // if checksum does not match or it did and we've toggled the relay then go back to wait
      i=0;
      goto waitLoop;
    }
    else if (onsig[0] == sample) {
      for(i = 1; i < sizeof onsig; ++i) {
        while (!Serial.available()) ;;
        if(onsig[i] == Serial.peek()) {
          Serial.read();
          continue;
        }
        else {// otherwise, start over
          i = 0;
          goto waitLoop;
        }
      }
      // Hi, Lo, Checksum for on signal
      while (!Serial.available()) ;;
      hi=Serial.read();
      while (!Serial.available()) ;;
      lo=Serial.read();
      while (!Serial.available()) ;;
      chk=Serial.read();
      
      if (chk == (hi ^ lo ^ 0x55)) {
        if (false == relay_status) {
          relay_status = true;
          digitalWrite(RELAY_PIN, HIGH);
          delay(500);
          LEDS.showColor(CRGB(0, 0, 64));
          delay(100);
          LEDS.showColor(CRGB(0, 0, 0));
          delay(150);
          LEDS.showColor(CRGB(0, 0, 64));
          delay(100);
          LEDS.showColor(CRGB(0, 0, 0));
        }
      }
      // if checksum does not match or it did and we've toggled the relay then go back to wait
      i=0;
      goto waitLoop;
    }
    else {
      // otherwise, start over
      i = 0;
      goto waitLoop;
    }
  }
  
  // Magic word received so... 
  // Hi, Lo, Checksum for lighting packets
  while (!Serial.available()) ;;
  hi=Serial.read();
  while (!Serial.available()) ;;
  lo=Serial.read();
  while (!Serial.available()) ;;
  chk=Serial.read();
  // if checksum does not match go back to wait
  if (chk != (hi ^ lo ^ 0x55)) {
    i=0;
    goto waitLoop;
  }

  // do some lighting!
  memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
  // read the transmission data and set LED values
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    byte r, g, b;    
    while(!Serial.available());
    r = Serial.read();
    while(!Serial.available());
    g = Serial.read();
    while(!Serial.available());
    b = Serial.read();
    leds[i].r = r;
    leds[i].g = g;
    leds[i].b = b;
  }
  // shows new values
 FastLED.show();
}
