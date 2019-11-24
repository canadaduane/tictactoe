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
    uint8_t _player;
    bool _shouldSave;
    WiFiManager _wm;
    WiFiManagerParameter *_player_param;
};

#endif
