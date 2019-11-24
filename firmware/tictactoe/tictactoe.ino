#include <Adafruit_NeoPixel.h>
#include <AceButton.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <ESP8266HTTPClient.h>

#define POLLING_INTERVAL_MS 500

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

typedef enum {
  Boot = 0,
  Play,
  Diagnose,
  Configure
} MajorMode;

typedef enum {
  Unattempted = 0,
  WifiStarted,
  WifiSuccess,
  WifiFailed,
  ServerSuccess,
  ServerFailed
} ConnectionStatus;

typedef enum {
  Transparent = 0,
  Dark,
  Lit,
  Rainbow
} pixelMode;

typedef struct {
  unsigned char pin;
  unsigned char pull;
  unsigned char released;
} buttonMapping;

typedef struct {
  pixelMode mode;
  uint32_t litColor;
  unsigned char cycle;
  unsigned char lightIndex;
} pixelState;

typedef struct {
  bool adminLit;
} pixelAdminState;

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

Adafruit_NeoPixel pixels =
  Adafruit_NeoPixel(NUMPIXELS, PIN_STRIP, NEO_GRB + NEO_KHZ800);

AceButton buttons[9];

WiFiClient client;
HTTPClient http;

WiFiManager wm;
WiFiManagerParameter custom_player("player", "Player (0 or 1)", "0", 1);

Ticker blinkTicker;
Ticker spinnerTicker;
Ticker enterConfigureModeButtonStateTicker;

ConnectionStatus connStatus = Unattempted;

MajorMode mode = Boot,
          nextMode = Boot;

// Which player/board am I? (0 or 1)
uint8_t I_AM = 0;
uint8_t enterConfigureModeButtonState = 0;
uint8_t spinnerState = 0;

// Magic at boot
uint16_t sparkleSwipeState = 0;
uint16_t sparkleHue[18];
bool autoConnectSuccess = false;

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

pixelAdminState boardAdmin[9] = {
    {false}, {false}, {false},
    {false}, {false}, {false},
    {false}, {false}, {false}
};

// Major Modes
void boot();
void play();
void diagnose();
void configure();

// Ticker Functions
void toggleLED();
void clearEnterConfigureModeButtonState();
void spinnerIncrement();

// Other Functions
void handleEvent(AceButton*, uint8_t, uint8_t);
uint32_t rainbowColor(unsigned char cycle);
void transitionTo(MajorMode newMode);
void lightAllPixels(uint32_t color);
void getBoardState(uint8_t localBoard, uint8_t remoteBoard);
void setBoardState(uint8_t localBoard, uint8_t remoteBoard);
void setupAccessPortal();
void loopAccessPortal();
void saveParamsCallback();
void configModeCallback(WiFiManager *myWiFiManager);

/*=== Setup and Loop Functions ===*/

void setup() 
{
  WiFi.mode(WIFI_STA);
  wm.setConfigPortalBlocking(false);
  wm.setCountry("US");
  wm.setConnectTimeout(5);
  wm.addParameter(&custom_player);
  wm.setSaveParamsCallback(saveParamsCallback);

  http.setReuse(true);
  
  pixels.begin();

  pinMode(LED_BUILTIN, OUTPUT); // PIN4 is same as on-board LED
  
  for (int i = 0; i < 9; i++) {
    pinMode(buttonMap[i].pin, buttonMap[i].pull);
    buttons[i].init(buttonMap[i].pin, buttonMap[i].released, i);
  }

  spinnerTicker.attach(0.2, spinnerIncrement);

  ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setLongPressDelay(500);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
}

void loop()
{
  // If we're transitioning to a new major mode,
  // handle the transition & set mode
  finalizeTransition();
  
  // track button states in all modes
  for (int i = 0; i < 9; i++) { buttons[i].check(); }

  switch(mode) {
    case Boot: boot(); break;
    case Play: play(); break;
    case Diagnose: diagnose(); break;
    case Configure: configure(); break;
  }
}

/*=== Major Mode Functions ===*/
void boot() {
//  transitionTo(Play);
  if (sparkleSwipeState == 0) {
    lightAllPixels(pixels.Color(5, 0, 0));
    autoConnectSuccess = wm.autoConnect("AutoConnectAP");
  } else {
    if (autoConnectSuccess) {
      for (int i = 0; i < 18; i++) sparkleHue[i] += 30 + i;
    } else {
      for (int i = 0; i < 18; i++) sparkleHue[i] += 5;      
    }
  }

  pixels.clear();
  for (int i = 0; i < 18; i++) {
    uint8_t lum = ((sin((double)sparkleSwipeState/1000 * PI - PI/2) - 1) / 2 + 1) * 50;
    pixels.setPixelColor(
      board[i % 2][i / 2].lightIndex,
      pixels.ColorHSV(sparkleHue[i], 255, lum)
    );
  }
  pixels.show();

  if (sparkleSwipeState++ > 2000) {
    transitionTo(Play);
  }
}

void lightAdminPixel(int index) {
  for (int p = 0; p < 2; p++) {
    pixels.setPixelColor(board[p][index].lightIndex, pixels.Color(0, 0, 30));
  }
}

void lightPixel(int player, int index) {
  switch (board[player][index].mode) {
    case Lit:
      pixels.setPixelColor(
        board[player][index].lightIndex,
        board[player][index].litColor
      );
      break;
    case Rainbow:
      pixels.setPixelColor(
        board[player][index].lightIndex,
        rainbowColor(board[player][index].cycle++)
      );
      break;
  }
}

unsigned long lastServerUpdate = 0;
bool serverSetState = true;

void play() {
  if (autoConnectSuccess) {
    unsigned long now = millis();
    if (now - lastServerUpdate > POLLING_INTERVAL_MS) {
      lastServerUpdate = now;
      if (serverSetState) {
        setBoardState(0, I_AM);
      } else {
        getBoardState(1, !I_AM);
      }
      serverSetState = !serverSetState;
    }
  }
  
  // Show LED Pixels
  pixels.clear();
  for (int p = 0; p < 2; p++) {
    for (int i = 0; i < 9; i++) {
      if (boardAdmin[i].adminLit) {
        lightAdminPixel(i);
      } else {
        lightPixel(p, i);
      }
    }
  }
  pixels.show();
}

void diagnose() {
  
}

void spinnerToAdminPixels() {
  static uint8_t circleMap[8] = {0, 1, 2, 5, 8, 7, 6, 3};
  uint8_t sp = spinnerState % 8;
  for (int i = sp; i < sp + 3; i++) boardAdmin[circleMap[i % 8]].adminLit = true;
  for (int i = sp + 3; i < sp + 8; i++) boardAdmin[circleMap[i % 8]].adminLit = false;
}

void configure() {
  spinnerToAdminPixels();

  loopAccessPortal();
  
  // Show LED Pixels
  pixels.clear();
  for (int p = 0; p < 2; p++) {
    for (int i = 0; i < 9; i++) {
      if (boardAdmin[i].adminLit) {
        lightAdminPixel(i);
      }
    }
  }
  pixels.show();  
}


/*=== Ticker Functions ===*/
// Toggle LED state
void toggleLED() {
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void clearBoardAdminPixels() {
  for (int i = 0; i < 9; i++) {
    boardAdmin[i].adminLit = false;
  }
}

// Reset enterConfigureModeButtonState counter
void clearEnterConfigureModeButtonState() {
  enterConfigureModeButtonState = 0;
  spinnerState = 0;
  clearBoardAdminPixels();
}

void spinnerIncrement() {
  spinnerState++;
}


/*=== Other Functions ===*/

void handleButtonBoot(int8_t index, uint8_t eventType) {
  switch (eventType) {
    case AceButton::kEventPressed:
      transitionTo(Play);
      break;
  }
}

void handleButtonPlay(int8_t index, uint8_t eventType) {
  switch (eventType) {
    case AceButton::kEventPressed:
      if (board[0][index].mode == Dark) {
        board[0][index].mode = Lit;
      } else {
        board[0][index].mode = Dark;
      }
      break;
    case AceButton::kEventLongPressed:
      if (board[0][index].mode != Rainbow) {
        board[0][index].mode = Rainbow;
      }
      break;
  }
}

void handleButtonDiagnose(int8_t index, uint8_t eventType) {
}

void handleButtonConfigure(int8_t index, uint8_t eventType) {
  // any button leaves WiFi configuration
  switch (eventType) {
    case AceButton::kEventReleased:
      ESP.restart();
      delay(2000);
      break;
    case AceButton::kEventLongPressed:
      if (index == 4) {
        // middle button
        wm.resetSettings();
        ESP.restart();
        delay(2000);
      }
      break;
  }
}

void handleButtonAll(int8_t index, uint8_t eventType) {
  switch (eventType) {
    case AceButton::kEventDoubleClicked:
      if (index < 3 && index == enterConfigureModeButtonState) {
        boardAdmin[index].adminLit = true;
        enterConfigureModeButtonState++;
        enterConfigureModeButtonStateTicker.once(3.0, clearEnterConfigureModeButtonState);
        if (index == 2) {
          enterConfigureModeButtonState = 0;
          enterConfigureModeButtonStateTicker.detach();
          transitionTo(Configure);
        }
      }
      break;
  }
}

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  int8_t i = button->getId();
  if (i >= 0 && i < 9) {
    handleButtonAll(i, eventType);
    switch (mode) {
      case Boot: handleButtonBoot(i, eventType); break;
      case Play: handleButtonPlay(i, eventType); break;
      case Diagnose: handleButtonDiagnose(i, eventType); break;
      case Configure: handleButtonConfigure(i, eventType); break;
    }
  }
}

uint32_t rainbowColor(unsigned char cycle) {
  return pixels.ColorHSV((uint16_t)cycle << 8, 255, 30);
}

void transitionTo(MajorMode newMode) {
  nextMode = newMode;
}

void finalizeTransition() {
  if (mode != nextMode) {
    if (nextMode == Play) {
      clearBoardAdminPixels();
    } else if (nextMode == Configure) {
      setupAccessPortal();
      spinnerState = 0;
    } else {
    }
    mode = nextMode;
  }
}

void lightAllPixels(uint32_t color) {
  pixels.clear();
  for (int i = 0; i < 18; i++) pixels.setPixelColor(board[i % 2][i / 2].lightIndex, color);
  pixels.show();
}

void getBoardState(uint8_t localBoard, uint8_t remoteBoard) {
  if (localBoard > 1) localBoard = 1;
  if (remoteBoard > 1) remoteBoard = 1;
  http.begin(client, "http://xoxoxotictactoe.herokuapp.com/" + String(remoteBoard));
  if (http.GET() == HTTP_CODE_OK) {
    String payload = http.getString();
    if (payload.length() == 17) {
      for (int i = 0; i < 18; i+=2) {
        board[localBoard][i/2].mode = (pixelMode)(payload.charAt(i) - '0');
      }
    }
  }
  http.end();  
}

void setBoardState(uint8_t localBoard, uint8_t remoteBoard) {
  if (localBoard > 1) localBoard = 1;
  if (remoteBoard > 1) remoteBoard = 1;
  http.begin(client,
    "http://xoxoxotictactoe.herokuapp.com/" +
      String(remoteBoard) + "/" +
      String((uint8_t)board[localBoard][0].mode) + "/" +
      String((uint8_t)board[localBoard][1].mode) + "/" +
      String((uint8_t)board[localBoard][2].mode) + "/" +
      String((uint8_t)board[localBoard][3].mode) + "/" +
      String((uint8_t)board[localBoard][4].mode) + "/" +
      String((uint8_t)board[localBoard][5].mode) + "/" +
      String((uint8_t)board[localBoard][6].mode) + "/" +
      String((uint8_t)board[localBoard][7].mode) + "/" +
      String((uint8_t)board[localBoard][8].mode)
  );
  http.PUT((const uint8_t)NULL, 0);
  http.end();  
}

void setupAccessPortal() {
  wm.startConfigPortal();
}

void loopAccessPortal() {
  wm.process();
}

void saveParamsCallback () {
  const char* value = custom_player.getValue();
  if (value[0] == '0') I_AM = 0;
  if (value[0] == '1') I_AM = 1;
}

// Gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  blinkTicker.attach(0.2, toggleLED);
}
