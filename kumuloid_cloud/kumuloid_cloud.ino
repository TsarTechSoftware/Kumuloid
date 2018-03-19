#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>

#define NUMPIXELS 144 // There are 72 pixels per strip and there are 2 strips.

#define DATAPIN    4
#define CLOCKPIN   5
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG); // This initializes the strip.

void setup() {
Serial.begin(9600); // Bluetooth Baud Rate
Serial.write("Welcome to Kumuloid!");
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)// This doesn't need to be here but 
  clock_prescale_set(clock_div_1);                   // I may want to switch to a cheaper
#endif                                               // platform next christmas.

  strip.begin(); // Starting SPI for the stip.
  strip.show();  // Clearing the strip.
}

byte buff[7];
char incomingByte = 0;           // This is the global byte that keeps track of all the data coming from bluetooth.
int     head  = 0, tail = -10;  // I coud make these variables local but I'm lazy.
uint32_t color_one = 0xFFFF33;  // The first in the two colors that can be applied to dotstar animations.
uint32_t color_two = 0x0000FF;
uint32_t globalBrightness = 255;// Keeps track of the desired brightness.

// **************************************************************************************************************//

// ************************************************  Color Fade  ************************************************//
/* This function takes one color and fades it to the other color. This excepts RGB colors.
percent = The percent of the second color that the output should be.
c1 = the first color.
c2 = the second color.*/
uint32_t colorFade(uint8_t percent, uint32_t c1, uint32_t c2){
  uint32_t r = map(percent, 0, 100, (c1 & 0xFF0000) >> 16, (c2 & 0xFF0000) >> 16);
  uint32_t b = map(percent, 0, 100, (c1 & 0x00FF00) >> 8, (c2 & 0x00FF00) >> 8);
  uint32_t g = map(percent, 0, 100, (c1 & 0x0000FF), (c2 & 0x0000FF));
  return ((r << 16) + (b << 8) + g);
  }

// ************************************************  Color Fade  ************************************************//

// **************************************************************************************************************//

// ************************************************ Color Static ************************************************//
/* This allows for just one color to be chosen and display as a single color.*/
void colorStatic(){
  while (1){
    // With each iteration of these loops, we have to read to see if the user has sent any
    // bytes as to modify the colors or brightnes.
    readByte();
    // break out of this function if the user sends an 'x'
    if(incomingByte == 'x'){return;}
    // Looping through the pixels and changing their color.
    for(uint8_t i = 0; i <= NUMPIXELS; i++){
      strip.setPixelColor(i, color_one);
      }
      // This sends the buffer we just made to the actual pixels
      strip.show();
      strip.setBrightness(globalBrightness);
    }
  }

// ************************************************ Color Static ************************************************//

// **************************************************************************************************************//

// ************************************************  Color Rain  ************************************************//

/* An unnecessarily complicated function to mimic a rain cloud*/

void colorRain(){
  uint32_t rainCloor = 0x1D9FBD; // The color of the rain drops
  uint32_t cloudCloor = 0x0C5D76; // The color of the cloud
  uint32_t rainCloorMask = 0x16708B; // a mask of the rain color for lightning.
  uint32_t flash = 0xD7F5FF; // lightning flash color
  uint8_t cloudBrightness = 100; // The brightness of the rain cloud
  strip.setBrightness(cloudBrightness);
  bool doFlash = false; // This will activate at random with a 1/900 chance per loop cycle.
  uint8_t dropPlacement; // Where the rain drop flickers will be.
  uint8_t flashTimer = 50; // How many cycles the lightning will last
  uint8_t flashTimerMask = 50; // A mask to be decremented per cycle and then reset to the above timer value.
  bool bigFlash = false; // 2 big flashes will happed per lightning strike
  uint16_t flashProbability = 900; // The bigger the number, the lower the probability... Ideally...
  uint16_t fleeshFeesh; // Boofaloo Willied Wangs
  while (1){
    readByte();
    if(incomingByte == 'x'){return;}
    delay(1);
    // More stuff I don't want to comment on unless my intructor makes me
    fleeshFeesh = random(0, flashProbability);
    if(fleeshFeesh == (int)flashProbability/2){}
    if(fleeshFeesh == (int)flashProbability/2 && doFlash == false){
      doFlash = true;
      
      strip.setBrightness(200);
      }
    bigFlash = false;
    if(doFlash == true){
      rainCloorMask = flash;
      flashTimerMask--;
      if(flashTimerMask%15 == 0){
        bigFlash = true;
        for(uint8_t i = 0; i <= NUMPIXELS; i++){
            strip.setPixelColor(i, flash);
        }
      }
      if(flashTimerMask == 0){
        flashTimerMask = flashTimer;
        doFlash = false;
        rainCloorMask = rainCloor;
        strip.setBrightness(globalBrightness);
        //Serial.print("Done Flash");
        }
      }
    if(bigFlash == false){
      for(uint8_t i = 0; i <= NUMPIXELS; i++){
            strip.setPixelColor(i, cloudCloor);
      }
    }
    dropPlacement = random(2, NUMPIXELS);
    strip.setPixelColor(dropPlacement, rainCloorMask);
    strip.setPixelColor(dropPlacement-1, rainCloorMask);
    strip.show();
    }
  }

// ************************************************  Color Rain  ************************************************//

// **************************************************************************************************************//

// ************************************************ Color Breath ************************************************//

// This function makes the lights fade in and out and cycle between two colors

void colorBreath(){
      uint32_t outColor = color_one;
      float percent = 0;
      float fadeAmount = .1;
      bool forward = false;
      uint8_t colorStay = 144;
      uint8_t breath = 11;
      bool breathFor = true;
      uint8_t breathTimer = 80;
      bool breathOut = false;
      while(1){
        readByte();
        if(incomingByte == 'x'){return;}
        strip.setBrightness((int)breath);
        if(breathFor == true && breathOut == false){breath += (10 * fadeAmount);}
        if(breathFor == false && breathOut == false){breath -= (2 * fadeAmount);}
        if((int)breath >= globalBrightness || (int)breath <= 10){
          if((int)breath <= 10){
            
            breathTimer--; 
            breathOut = false;
            if(breathTimer > 0){breathOut = true;}
            else{breathTimer = 80;}
          }
          
          if(breathOut == false){breathFor = !breathFor;}
          }
        
        outColor = colorFade((int)percent, color_one, color_two);
        for(uint8_t i = 0; i <= NUMPIXELS; i++){
          strip.setPixelColor(i, outColor);
          }
        strip.show();
        delay(1);
        if(percent <= 0 || percent >= 100){
          colorStay--;
          if(colorStay == 0){
          forward = !forward;
          colorStay = 144;
            }
          else{continue;}
          
          }
        
        if(forward == true){
          percent += fadeAmount;
          //Serial.print(outColor, HEX);
          //Serial.print("");
          }
        else{
          percent -= fadeAmount;
          //Serial.print(outColor, HEX);
          //Serial.print("");
          }
        }
  
  }

// ************************************************ Color Breath ************************************************//

// **************************************************************************************************************//

// ************************************************  Color Walk  ************************************************//

// The strip 'walks' from one color to the other

void colorWalk(){
      head = 0;
      tail = -143;
      uint32_t outColor = color_one;
      float percent = 0;
      float fadeAmount = .1;
      bool forward = false;
      uint8_t colorStay = 144;
      while(1){
        readByte();
        if(incomingByte == 'x'){return;}
        outColor = colorFade((int)percent, color_one, color_two);
        strip.setPixelColor(head, outColor); // 'On' pixel at head
        strip.setPixelColor(tail, outColor);     // 'Off' pixel at tail
        strip.show();                     // Refresh strip
        delay(2);                        // Pause 2 milliseconds (~50 FPS)
      
        if(++head >= NUMPIXELS) {         // Increment head index.  Off end of strip?
          head = 0;                       //  Yes, reset head index to start
        }
        if(++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index

        
        if(percent <= 0 || percent >= 100){
          colorStay--;
          if(colorStay == 0){
          forward = !forward;
          colorStay = 144;
            }
          else{continue;}
          
          }
        
        if(forward == true){
          percent += fadeAmount;
          }
        else{
          percent -= fadeAmount;
          }
      }
  }

// ************************************************  Color Walk  ************************************************//

// **************************************************************************************************************//

// ************************************************ Data Reading ************************************************//

void readByte(){
  if(Serial.available() > 0){
    incomingByte = Serial.read();
    }
    if(incomingByte == 'Q'){get_GRB_and_brightness();}
  }

void get_GRB_and_brightness(){
  byte tempOne = incomingByte;
  if(Serial.available() > 0){
    Serial.readBytes(buff, 7);
    }
  //color_one = (buff[0] << 16) + (buff[1] << 8) + (buff[2]);
  //color_two = (buff[3] << 16) + (buff[4] << 8) + (buff[5]);

  // The above code would be preferable but in C++, shifting uses the value
  // that we're shifting and shifts it within the same register.
  // Meaning, a char cannot be shifted 16 <<. Fix this later maybe.
  color_one = 0;
  color_two = 0;
  for(uint8_t i = 0; i < 2; i++){
    color_one += buff[i];
    color_one = color_one << 8;

    color_two += buff[i + 3];
    color_two = color_two << 8;
    }
    color_one += buff[2];
    color_two += buff[5];
  globalBrightness = buff[6];
  incomingByte = tempOne;
  }

// ************************************************ Data Reading ************************************************//

// **************************************************************************************************************//

void loop() {
  readByte();
  if(incomingByte == 'x'){readByte();}
  if(incomingByte == 'a'){colorRain();}
  if(incomingByte == 'b'){colorBreath();}
  if(incomingByte == 'c'){colorWalk();}
  if(incomingByte == 'd'){colorStatic();}
}
