#include "PixelState.h"
#include "Pins.h"

#define NUMPIXELS 18
#define PIN_STRIP PIN6

Adafruit_NeoPixel pixels =
  Adafruit_NeoPixel(NUMPIXELS, PIN_STRIP, NEO_GRB + NEO_KHZ800);

PixelState board[2][9] = {
  // Player 1
  {
    {Dark,pixels.Color(50,0,0),0,17},
    {Dark,pixels.Color(50,0,0),0,15},
    {Dark,pixels.Color(50,0,0),0,13},
    
    {Dark,pixels.Color(50,0,0),0,6},
    {Dark,pixels.Color(50,0,0),0,8},
    {Dark,pixels.Color(50,0,0),0,10},
    
    {Dark,pixels.Color(50,0,0),0,5},
    {Dark,pixels.Color(50,0,0),0,3},
    {Dark,pixels.Color(50,0,0),0,1}
  },
  // Player 2
  {
    {Dark,pixels.Color(0,50,0),0,16},
    {Dark,pixels.Color(0,50,0),0,14},
    {Dark,pixels.Color(0,50,0),0,12},
    
    {Dark,pixels.Color(0,50,0),0,7},
    {Dark,pixels.Color(0,50,0),0,9},
    {Dark,pixels.Color(0,50,0),0,11},
    
    {Dark,pixels.Color(0,50,0),0,4},
    {Dark,pixels.Color(0,50,0),0,2},
    {Dark,pixels.Color(0,50,0),0,0}
  }
};
