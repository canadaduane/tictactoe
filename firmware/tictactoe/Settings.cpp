#include "Settings.h"

Settings::Settings() {
  _player = 0;
  _player_param = new WiFiManagerParameter("player", "Player (0 or 1)", "0", 1);
}

uint8_t Settings::getPlayer() {
  return _player;
}

uint8_t Settings::getOpponent() {
  return !_player;
}

void Settings::setupAccessPortal() {
  _wm.setConfigPortalBlocking(false);
  _wm.setCountry("US");
  _wm.setConnectTimeout(5);
  _wm.addParameter(_player_param);
  //_wm.setSaveParamsCallback(saveParamsCallback);

  _wm.startConfigPortal();
}

void Settings::loopAccessPortal() {
  _wm.process();
}

void Settings::saveParamsCallback() {
  const char* value = _player_param->getValue();
  if (value[0] == '0') _player = 0;
  if (value[0] == '1') _player = 1;
}

bool Settings::connect() {
  return _wm.autoConnect("AutoConnectAP");
}

void Settings::reset() {
  _wm.resetSettings();
}
