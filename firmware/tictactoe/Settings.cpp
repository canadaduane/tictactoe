#include "Settings.h"
#include <FS.h>

Settings::Settings() {
  _player = 0;
  _shouldSave = false;
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

  std::function<void()> lambdaSaveParams = std::bind(&Settings::saveParamsCallback, this);
  _wm.setSaveParamsCallback(lambdaSaveParams);

  _wm.startConfigPortal();
}

void Settings::loopAccessPortal() {
  _wm.process();
  if (_shouldSave) {
    const char* value = _player_param->getValue();
    if (value[0] == '0') _player = 0;
    if (value[0] == '1') _player = 1;
    // TODO: save to FS
  }
}

void Settings::saveParamsCallback() {
  _shouldSave = true;
}

bool Settings::connect() {
  return _wm.autoConnect("AutoConnectAP");
}

void Settings::reset() {
  _wm.resetSettings();
}
