#include <Adafruit_NeoPixel.h>
#include <AceButton.h>
using namespace ace_button;

#define PIN0 16
#define PIN1 5
#define PIN2 4
#define PIN3 0
// PIN4 is the LED
#define PIN5 14
#define PIN6 12
#define PIN7 13
#define PIN8 15
#define PINRX 3
#define PINTX 1

#define NUMPIXELS 18
#define PIN_STRIP PIN6

Adafruit_NeoPixel pixels =
  Adafruit_NeoPixel(NUMPIXELS, PIN_STRIP, NEO_GRB + NEO_KHZ800);

typedef struct {
  unsigned char pin;
  unsigned char pull;
  unsigned char released;
} buttonMapping;

buttonMapping buttonMap[9] = {
  {PIN1,INPUT_PULLUP,HIGH},
  {PINRX,FUNCTION_3,HIGH},
  {PINTX,FUNCTION_3,HIGH},
  
  {PIN2,INPUT_PULLUP,HIGH},
  {PIN5,INPUT_PULLUP,HIGH},
  {PIN8,INPUT_PULLDOWN_16,LOW},
  
  {PIN3,INPUT_PULLUP,HIGH},
  {PIN0,INPUT_PULLDOWN_16,LOW},
  {PIN7,INPUT_PULLUP,HIGH}
};

AceButton buttons[9];

typedef enum {
  Dark = 0,
  Lit,
  Rainbow
} pixelMode;

typedef struct {
  pixelMode mode;
  uint32_t litColor;
  unsigned char cycle;
  unsigned char lightIndex;
} pixelState;

pixelState board[2][9] = {
  // Player 1
  {
    {Dark,pixels.Color(50,0,0),0,17}, {Dark,pixels.Color(50,0,0),0,15}, {Dark,pixels.Color(50,0,0),0,13},
    {Dark,pixels.Color(50,0,0),0,6}, {Dark,pixels.Color(50,0,0),0,8}, {Dark,pixels.Color(50,0,0),0,10},
    {Dark,pixels.Color(50,0,0),0,5}, {Dark,pixels.Color(50,0,0),0,3}, {Dark,pixels.Color(50,0,0),0,1}
  },
  // Player 2
  {
    {Dark,pixels.Color(0,50,0),0,16}, {Dark,pixels.Color(0,50,0),0,14}, {Dark,pixels.Color(0,50,0),0,12},
    {Dark,pixels.Color(0,50,0),0,7}, {Dark,pixels.Color(0,50,0),0,9}, {Dark,pixels.Color(0,50,0),0,11},
    {Dark,pixels.Color(0,50,0),0,4}, {Dark,pixels.Color(0,50,0),0,2}, {Dark,pixels.Color(0,50,0),0,0}
  }
};

void handleEvent(AceButton*, uint8_t, uint8_t);

void setup() 
{
  pixels.begin();

  pinMode(LED_BUILTIN, OUTPUT); // PIN4 is same as on-board LED
  
  for (int i = 0; i < 9; i++) {
    pinMode(buttonMap[i].pin, buttonMap[i].pull);
    buttons[i].init(buttonMap[i].pin, buttonMap[i].released, i);
  }

  ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setLongPressDelay(500);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
}

uint32_t rainbowColor(unsigned char cycle) {
  return pixels.ColorHSV((uint16_t)cycle << 8, 255, 50);
//  if (cycle <= 85) return pixels.Color(50, 0, 0);
//  if (cycle <= 170) return pixels.Color(0, 50, 0);
//  if (cycle <= 255) return pixels.Color(0, 0, 50);
}

void loop()
{
  // buttons
  for (int i = 0; i < 9; i++) {
    buttons[i].check();
  }

  // pixels
  pixels.clear();
  for (int p = 0; p < 2; p++) {
    for (int i = 0; i < 9; i++) {
      switch (board[p][i].mode) {
        case Lit:
          pixels.setPixelColor(board[p][i].lightIndex, board[p][i].litColor);
          break;
        case Rainbow:
          pixels.setPixelColor(board[p][i].lightIndex, rainbowColor(board[p][i].cycle++));
          break;
      }
    }
  }
  pixels.show();
  
  delay(20);
}

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  int8_t i = button->getId();
  if (i >= 0 && i < 9) {
    switch (eventType) {
      case AceButton::kEventPressed:
        if (board[0][i].mode == Dark) {
          board[0][i].mode = Lit;
        } else {
          board[0][i].mode = Dark;
        }
        break;
      case AceButton::kEventLongPressed:
        if (board[0][i].mode != Rainbow) {
          board[0][i].mode = Rainbow;
        }
        break;
    }
  }
}
