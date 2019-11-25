#include "Settings.h"
#include <FS.h>

TicTacToeConfig tconfig;

Settings::Settings() {
  _initializedWiFi = false;
  _configLoaded = false;
}

void Settings::initializeWiFi() {
  if (!_initializedWiFi) {
    _cm.setAPName("TicTacToe");
    _cm.setAPFilename("/index.html");
    _cm.addParameter("player", &tconfig.player);
    
    _initializedWiFi = true;
  }
}

void Settings::readFsConfig() {
  _configLoaded = true;
  
//  Serial.begin(115200);
//
//  if (SPIFFS.begin()) {
//    Serial.println("mounted file system");
//    if (SPIFFS.exists("/config.json")) {
//      //file exists, reading and loading
//      Serial.println("reading config file");
//      File configFile = SPIFFS.open("/player.txt", "r");
//      if (configFile) {
//        Serial.println("opened config file");
//        String data = configFile.readString();
//
//        Serial.print("config file size: ");
//        Serial.println(data.length());
//
//        if (data.length() >= 1) {
//          if (data.charAt(0) == '0') _player = 0;
//          if (data.charAt(0) == '1') _player = 1;
//          Serial.print("player is ");
//          Serial.println(_player, HEX);
//        }
//
//        configFile.close();
//      }
//    }
//  } else {
//    Serial.println("failed to mount FS");
//  }
}

uint8_t Settings::getPlayer() {
  if (!_configLoaded) readFsConfig();
  return tconfig.player;
}

uint8_t Settings::getOpponent() {
  return !tconfig.player;
}

void Settings::setupAccessPortal() {
  initializeWiFi();
  _cm.begin(tconfig);
}

void Settings::loopAccessPortal() {
  _cm.loop();
}

bool Settings::connect() {
  setupAccessPortal();
  return WiFi.status() == WL_CONNECTED;
//  return _cm.wifiConnected();
//  initializeWiFi();
//  return _wm.autoConnect("AutoConnectAP");
}

void Settings::reset() {
  _cm.clearWifiSettings(false);
}
