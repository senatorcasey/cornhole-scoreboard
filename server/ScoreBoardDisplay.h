#ifndef SCOREBOARDDISPLAY
#define SCOREBOARDDISPLAY

#include <Arduino.h>
#include "TM1637Display.h"

#define HOMEAWAY 0
#define AWAYHOME 1

class ScoreBoardDisplay {
  private:  
    // scoring related variables
    uint8_t homeScore = 0;
    uint8_t awayScore = 0;

    // display related variables
    uint8_t clk;
    uint8_t dio;
    TM1637Display display;
    uint8_t displayOrder = HOMEAWAY;

    // animation related variables
    static uint8_t shortAnimation[][4];
    bool resetAnimation = true;
    uint8_t colonFlag = 0b01000000;
    unsigned long colonTimeMS = 0;
    unsigned long colonDelayMS = 1000;

    void updateDisplay();
    void playAnimation();

  public:
    ScoreBoardDisplay(uint8_t clockPin, uint8_t dataPin);
    void setup();
    void loop();
    uint8_t incrementHomeScore();
    uint8_t decrementHomeScore();
    uint8_t incrementAwayScore();
    uint8_t decrementAwayScore();
    uint8_t getHomeScore() { return homeScore; }
    uint8_t getAwayScore() { return awayScore; }
    void setAwayScore(uint8_t newScore);
    void setHomeScore(uint8_t newScore);
    void setDisplayOrder(uint8_t newOrder);
};

#endif
