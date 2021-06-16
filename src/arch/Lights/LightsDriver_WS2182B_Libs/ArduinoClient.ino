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
#define BYTE_SETMODE 0x01
#define BYTE_SETLEDS 0x02

#define PIN        7
#define NUMPIXELS 20

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
PacketSerial pkSerial;
byte buffer[6]; //max size of 6
char bufferStr[6];

void setup() {
  Serial.begin(9600);
  //pkSerial.setStream(&Serial);
  //pkSerial.begin(9600);
  //pkSerial.setPacketHandler(&handlePacket);
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  pixels.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  
}

void loop() {
  /*for(int i=0; i<NUMPIXELS; i++) {

    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.show();
    delay(DELAYVAL);
  }*/
  //Serial.print("aa");
  //pkSerial.update();

  while (Serial.available()>2)
  {
    Serial.readBytesUntil(0,buffer,6);
    /*for(int i=0;i<6;i++){
      Serial.print(buffer[i]);
    }*/
    //Serial.print(buffer[5]);
    if (buffer[0]==BYTE_SETLEDS)
      setLEDs(buffer[2],buffer[3],buffer[4],buffer[5]);
  }
}

void setLEDs(int r,int g,int b,int percent)
{
    digitalWrite(LED_BUILTIN, HIGH);
    pixels.clear();
    int percentToShow = SCALE(percent,0,255,0,NUMPIXELS);
    for(int i=0; i<percentToShow; i++) {
  
      pixels.setPixelColor(i, pixels.Color(r, g, b));
    }
    pixels.show();
}

void handlePacket(const void* sender, const uint8_t* buffer, size_t size)
{
  pkSerial.send(buffer,size);
  //Serial.print("AAAAAAAAAAAAAAAAAAAAAAAA");
  if (buffer[0]==BYTE_SETLEDS)
  {
    

    digitalWrite(LED_BUILTIN, LOW);
  }
}
