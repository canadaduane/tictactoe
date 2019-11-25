#ifndef __PIXEL_STATE_H__
#define __PIXEL_STATE_H__

#include <Adafruit_NeoPixel.h>


typedef enum {
  Transparent = 0,
  Dark,
  Lit,
  Rainbow
} PixelMode;

typedef struct {
  PixelMode mode;
  uint32_t litColor;
  unsigned char cycle;
  unsigned char lightIndex;
} PixelState;

extern Adafruit_NeoPixel pixels;
extern PixelState board[2][9];

#endif
