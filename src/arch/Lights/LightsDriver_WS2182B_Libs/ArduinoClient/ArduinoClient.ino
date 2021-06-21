#include <Adafruit_NeoPixel.h>
#include <PacketSerial.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
//Stolen from RageUtil
/**
 * @brief Scales x so that l1 corresponds to l2 and h1 corresponds to h2.
 *
 * This does not modify x, so it MUST assign the result to something!
 * Do the multiply before the divide to that integer scales have more precision.
 *
 * One such example: SCALE(x, 0, 1, L, H); interpolate between L and H.
 */
#define SCALE(x, l1, h1, l2, h2)  (((x) - (l1)) * ((h2) - (l2)) / ((h1) - (l1)) + (l2))
#define MODE_TRAVEL 0x01
#define MODE_REGULAR 0x02
#define MODE_REVERSE 0x03
#define MODE_RAINBOW 0x04

#define NUMPIXELS 20

//Black cabinets have 6 spires so we should really be using 6. And there should be some way to determine which ones belong in p1 and which ones belong in p2, maybe a struct or class will help.
#define NUM_SPIRES 2
Adafruit_NeoPixel spires[NUM_SPIRES] = {Adafruit_NeoPixel(NUMPIXELS, 6, NEO_GRB + NEO_KHZ800), Adafruit_NeoPixel(NUMPIXELS, 7, NEO_GRB + NEO_KHZ800)};
//Adafruit_NeoPixel spire_P2(NUMPIXELS, 8, NEO_GRB + NEO_KHZ800);
PacketSerial pkSerial;
byte buffer[6]; //max size of 6
char bufferStr[6];

struct RGB {
  byte r;
  byte g;
  byte b;
};


void setup() {
  Serial.begin(9600);
  //pkSerial.setStream(&Serial);
  //pkSerial.begin(9600);
  //pkSerial.setPacketHandler(&handlePacket);
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  for (byte i=0;i<NUM_SPIRES;i++)
    spires[i].begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  
}

void loop() {
  
  /*for (byte p=0;p<NUM_SPIRES;p++){
    spires[p].clear();
    uint32_t color = p==1 ? spires[p].Color(0, 50, 0) : spires[p].Color(0, 0, 50);
    for(int i=0; i<NUMPIXELS; i++) {
  
      spires[p].setPixelColor(i, color);
      spires[p].show();
      delay(100);
    }
  }
  return;*/
  
  //Serial.print("aa");
  //pkSerial.update();

  //setLEDsRainbow(128);
  while (Serial.available()>2)
  {
    Serial.readBytesUntil(0,buffer,6);
    int spNum = buffer[1]-1;
    //Serial.print(spNum);
    /*for(int i=0;i<6;i++){
      Serial.print(buffer[i]);
    }*/
    //Serial.print(buffer[5]);
    if (buffer[0]==MODE_REGULAR)
      setLEDs(&spires[spNum],buffer[2],buffer[3],buffer[4],buffer[5]);
    else if (buffer[0]==MODE_REVERSE)
      setLEDsReversed(&spires[spNum],buffer[2],buffer[3],buffer[4],buffer[5]);
    else if (buffer[0] == MODE_TRAVEL)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      Adafruit_NeoPixel *spire = &spires[spNum];
      spire->clear();
      spire->setPixelColor(SCALE(buffer[5],1,255,0,NUMPIXELS), spire->Color(buffer[2],buffer[3],buffer[4]));
      spire->show();
    }
    else if (buffer[0]==MODE_RAINBOW)
      setLEDsRainbow(&spires[spNum],buffer[5]);
  digitalWrite(LED_BUILTIN,LOW);
  }
}

void setLEDs(Adafruit_NeoPixel *spire, int r,int g,int b,int percent)
{
    digitalWrite(LED_BUILTIN, HIGH);
    spire->clear();
    int percentToShow = SCALE(percent,1,255,0,NUMPIXELS);
    for(int i=0; i<percentToShow; i++) {
  
      spire->setPixelColor(i, spire->Color(r, g, b));
    }
    spire->show();
}
void setLEDsReversed(Adafruit_NeoPixel *spire, int r,int g,int b,int percent)
{
    digitalWrite(LED_BUILTIN, HIGH);
    spire->clear();
    int percentToShow = SCALE(percent,1,255,0,NUMPIXELS);
    for(int i=NUMPIXELS; i>percentToShow; i--) {
  
      spire->setPixelColor(i, spire->Color(r, g, b));
    }
    spire->show();
}

/*float* hsv2rgb(float h, float s, float b, float* rgb) {
  rgb[0] = b * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[1] = b * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[2] = b * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  //return rgb;
}*/

uint32_t firstPixelHue = 0;
void setLEDsRainbow(Adafruit_NeoPixel *spire, int percent)
{
  spire->clear();
  int percentToShow = SCALE(percent,1,255,0,NUMPIXELS);
  for (int i = 0; i < NUMPIXELS; i++) { //We still want to calculate for the entire span of spire_P1 even if they aren't shown
    int pixelHue = firstPixelHue + (i * 65536L / NUMPIXELS);
    //This calculation takes so long that the arduino starts to drop commands...
    if (i<percentToShow)
      spire->setPixelColor(i,Adafruit_NeoPixel::ColorHSV(pixelHue,255,10));
    
  }
  spire->show();
  firstPixelHue += 100; // Advance just a little along the color wheel
}


void handlePacket(const void* sender, const uint8_t* buffer, size_t size)
{
  pkSerial.send(buffer,size);
  //Serial.print("AAAAAAAAAAAAAAAAAAAAAAAA");
  if (buffer[0]==MODE_REGULAR)
  {
    

    digitalWrite(LED_BUILTIN, LOW);
  }
}
