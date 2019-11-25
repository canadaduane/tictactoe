#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <ConfigManager.h>

typedef struct {
  uint8_t player;
} TicTacToeConfig;

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
    ConfigManager _cm;

    void initializeWiFi();
    void readFsConfig();
};

#endif
