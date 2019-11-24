#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <WiFiManager.h>

class Settings {
  public:
    Settings();

    uint8_t getPlayer();
    uint8_t getOpponent();

    void setupAccessPortal();
    void loopAccessPortal();
    void saveParamsCallback();

    bool connect();
    void reset();

  private:
    bool _initializedWiFi;
    bool _configLoaded;
    uint8_t _player;
    WiFiManager _wm;

    void initializeWiFi();
    void readFsConfig();
};

#endif
