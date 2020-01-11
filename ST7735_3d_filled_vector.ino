// ST7735 library example
// 3D Filled Vector Graphics
// (c) 2019 Pawel A. Hernik
// YouTube video: https://youtu.be/5y28ipwQs-E 

/*
 ST7735 128x160 1.8" LCD pinout (header at the top, from left):
 #1 LED   -> 3.3V
 #2 SCK   -> SCL/D13/PA5
 #3 SDA   -> MOSI/D11/PA7
 #4 A0/DC -> D8/PA1  or any digital
 #5 RESET -> D9/PA0  or any digital
 #6 CS    -> D10/PA2 or any digital
 #7 GND   -> GND
 #8 VCC   -> 3.3V
*/

/*
 Implemented features:
 - optimized rendering without local framebuffer, in STM32 case 1 to 32 lines buffer can be used
 - pattern based background
 - 3D starfield
 - no floating point arithmetic
 - no slow trigonometric functions
 - rotations around X and Y axes
 - simple outside screen culling
 - rasterizer working for all convex polygons
 - backface culling
 - visible faces sorting by Z axis
*/

#define SCR_WD  128
#define SCR_HT  160

#define WD_3D   128
#define HT_3D   128

#include <SPI.h>
#include <Adafruit_GFX.h>

#if (__STM32F1__) // bluepill
#define TFT_CS  PA2
#define TFT_DC  PA1
#define TFT_RST PA0
#include <Arduino_ST7735_STM.h>
#else
#define TFT_CS 10
#define TFT_DC  8
#define TFT_RST 9
//#include <Arduino_ST7735_Fast.h>
#endif

Arduino_ST7735 lcd = Arduino_ST7735(TFT_DC, TFT_RST, TFT_CS);

// ------------------------------------------------
#define BUTTON PB9
int buttonState;
int prevState = HIGH;
long btDebounce    = 30;
long btMultiClick  = 600;
long btLongClick   = 500;
long btLongerClick = 2000;
long btTime = 0, btTime2 = 0;
int clickCnt = 1;

// 0=idle, 1,2,3=click, -1,-2=longclick
int checkButton()
{
  int state = digitalRead(BUTTON);
  if( state == LOW && prevState == HIGH ) { btTime = millis(); prevState = state; return 0; } // button just pressed
  if( state == HIGH && prevState == LOW ) { // button just released
    prevState = state;
    if( millis()-btTime >= btDebounce && millis()-btTime < btLongClick ) { 
      if( millis()-btTime2<btMultiClick ) clickCnt++; else clickCnt=1;
      btTime2 = millis();
      return clickCnt; 
    } 
  }
  if( state == LOW && millis()-btTime >= btLongerClick ) { prevState = state; return -2; }
  if( state == LOW && millis()-btTime >= btLongClick ) { prevState = state; return -1; }
  return 0;
}

int prevButtonState=0;

int handleButton()
{
  prevButtonState = buttonState;
  buttonState = checkButton();
  return buttonState;
}

// --------------------------------------------------------------------------
int bgMode=3;
int object=1;
int bfCull=1;

#include "pat2.h"
#include "pat7.h"
#include "pat8.h"
#include "gfx3d.h"

void setup() 
{
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);
  lcd.init();
  lcd.fillScreen(BLACK);
  lcd.setTextColor(YELLOW,BLACK);
  initStars();
}

unsigned int ms,msMin=1000,msMax=0;
char txt[30];

void loop()
{
  handleButton();
  if(buttonState<0 && prevButtonState>=0 && ++bgMode>4) bgMode=0;
  if(buttonState>0) {
    if(++object>3) object=0;
    msMin=1000;
    msMax=0;
  }
  switch(object) {
    case 0:
      numVerts  = numVerts1;
      verts     = (int16_t*)verts1;
      numQuads  = numQuads1;
      quads     = (int16_t*)quads1;
      quadColor = (uint16_t*)quadColor1;
      bfCull    = 1;
      break;
   case 1:
      numVerts  = numVerts2;
      verts     = (int16_t*)verts2;
      numQuads  = numQuads2;
      quads     = (int16_t*)quads2;
      quadColor = (uint16_t*)quadColor2;
      bfCull    = 1;
      break;
   case 2:
   default:
      numVerts  = numVerts3;
      verts     = (int16_t*)verts3;
      numQuads  = numQuads3;
      quads     = (int16_t*)quads3;
      quadColor = (uint16_t*)quadColor3;
      bfCull    = 1;
      break;
   case 3:
      numVerts  = numVerts4;
      verts     = (int16_t*)verts4;
      numQuads  = numQuads4;
      quads     = (int16_t*)quads4;
      quadColor = (uint16_t*)quadColor4;
      bfCull    = 0;
      break;
  }
  ms=millis();
  render3D();
  ms=millis()-ms;
  if(ms<msMin) msMin=ms;
  if(ms>msMax) msMax=ms;
  snprintf(txt,30,"%d ms     %d fps ",ms,1000/ms);
  lcd.setTextColor(YELLOW,BLACK); lcd.setCursor(0,SCR_HT-28); lcd.print(txt);
  snprintf(txt,30,"%d-%d ms  %d-%d fps   ",msMin,msMax,1000/msMax,1000/msMin);
  lcd.setTextColor(GREEN,BLACK); lcd.setCursor(0,SCR_HT-18); lcd.print(txt);
  snprintf(txt,30,"total/vis %d / %d   ",numQuads,numVisible);
  lcd.setTextColor(MAGENTA,BLACK); lcd.setCursor(0,SCR_HT-8); lcd.print(txt);
}

