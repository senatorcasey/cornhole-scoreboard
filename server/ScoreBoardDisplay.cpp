#include "ScoreBoardDisplay.h"

uint8_t ScoreBoardDisplay::shortAnimation[][4] = {
  { SEG_A, 0, 0, 0 },
  { 0, SEG_A, 0, 0 },
  { 0, 0, SEG_A, 0 },
  { 0, 0, 0, SEG_A },
  { 0, 0, 0, SEG_B },
  { 0, 0, 0, SEG_C },
  { 0, 0, 0, SEG_D },
  { 0, 0, SEG_D, 0 },
  { 0, SEG_D, 0, 0 },
  { SEG_D, 0, 0, 0 },
  { SEG_E, 0, 0, 0 },
  { SEG_F, 0, 0, 0 }
};


ScoreBoardDisplay::ScoreBoardDisplay(uint8_t clockPin, uint8_t dataPin) :
  clk(clockPin),
  dio(dataPin),
  display(clockPin, dataPin) {
  // there isn't anything else to do
} // end ScoreBoardDisplay(clk, dio)


void ScoreBoardDisplay::playAnimation() {
  // display each of the arrays stored in shortAnimation
  for (int i = 0; i < (sizeof(shortAnimation) / sizeof(shortAnimation[0])); i++) {
    display.setSegments(shortAnimation[i]);
  }

  // flip the animation flag here so that it doesn't keep playing
  resetAnimation = false;
} // end playAnimation()


void ScoreBoardDisplay::updateDisplay() {
  // calculate the digits to display
  int scoreToDisplay;
  if (displayOrder == HOMEAWAY) scoreToDisplay = (homeScore * 100) + awayScore;
  else scoreToDisplay = (awayScore * 100) + homeScore;

  // if scoreToDisplay is greater than zero,
  // then play the animation next time it is 0:0
  if (scoreToDisplay) {
    resetAnimation = true;
  } else if (resetAnimation) {
    // the animation is waiting to be played
    playAnimation();
  }

  // finally update the TM1637
  display.showNumberDecEx(scoreToDisplay, colonFlag, true, 4, 0);
} // end updateDisplay()


void ScoreBoardDisplay::setup() {
  // set the pin modes and put something on the display
  pinMode(clk, OUTPUT);
  pinMode(dio, OUTPUT);
  display.setBrightness(0x0f);
  updateDisplay();
} // end setup()


void ScoreBoardDisplay::loop() {
  // if it's time to flip the colon
  if (millis() - colonTimeMS >= colonDelayMS) {
    // flip the flag, update the time, and update the display
    colonFlag = colonFlag ? 0 : 0b01000000;
    colonTimeMS = millis();
    updateDisplay();
  }
} // end loop()


uint8_t ScoreBoardDisplay::incrementHomeScore() {
  // don't go past 99 points
  if (homeScore < 99) {
    homeScore++;
    updateDisplay();
  }
}


uint8_t ScoreBoardDisplay::decrementHomeScore() {
  // don't go below 0 points
  if (homeScore) {
    homeScore--;
    updateDisplay();
  }
}


uint8_t ScoreBoardDisplay::incrementAwayScore() {
  // don't go past 99 points
  if (awayScore < 99) {
    awayScore++;
    updateDisplay();
  }
}


uint8_t ScoreBoardDisplay::decrementAwayScore() {
  // don't go below 0 points
  if (awayScore) {
    awayScore--;
    updateDisplay();
  }
}


void ScoreBoardDisplay::setHomeScore(uint8_t newScore) {
  if (0 <= newScore && newScore <= 99) {
    homeScore = newScore;
    updateDisplay();
  }
}


void ScoreBoardDisplay::setAwayScore(uint8_t newScore) {
  if (0 <= newScore && newScore <= 99) {
    awayScore = newScore;
    updateDisplay();
  }
}


void ScoreBoardDisplay::setDisplayOrder(uint8_t newOrder) {
  displayOrder = newOrder;
}



