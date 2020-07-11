#include <AceButton.h>
#include <ESP8266WiFi.h>

using namespace ace_button;

// WiFi credentials
const char *ssid = "cornhole";
const char *password = "first221";

// usefull for debugging how long the loop function runs
unsigned long loopTime = 0;
bool printLoopTime = false;


const int LED_PIN = LED_BUILTIN;
const int BUTTON_A_PIN = D3;
const int BUTTON_B_PIN = D4;
const int LED_ON = HIGH;
const int LED_OFF = LOW;

// declare the button stuff
AceButton buttonA(BUTTON_A_PIN);
AceButton buttonB(BUTTON_B_PIN);
void handleButton(AceButton*, uint8_t, uint8_t);


void setup() {
  Serial.begin(115200);
  Serial.println();

  // turn off the built-in LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  // how to identify buttons?
  Serial.printf("LED_PIN = LED_BUILTIN = %d\n", LED_PIN);
  Serial.printf("buttonA (%d) is attached to BUTTON_A_PIN (%d)\n", &buttonA, BUTTON_A_PIN);
  Serial.printf("buttonB (%d) is attached to BUTTON_B_PIN (%d)\n", &buttonB, BUTTON_B_PIN);

  // modify the default button config for both buttons
  /* ButtonConfig setup (because it's confusing AF with all the possible options)
      setEventHandler(handleButton) is perhaps the most obvious option
      ButtonConfig::kEventReleased is always there by default, doesn't need to be turned on
      ButtonConfig::kFeatureRepeatPress will be called after 1000ms, and every 200ms thereafter
      ButtonConfig::kFeatureSuppressAll just makes sure the callback function is only called when expected
      Ignore ButtonConfig::kFeatureClick and kFeatureDoubleClick, they slow reaction times down and lose some events
      Ignore ButtonConfig::kFeatureLongPress because it's redundant with kFeatureRepeatPress, but doesn't repeat
  */
  ButtonConfig* bc = ButtonConfig::getSystemButtonConfig();
  bc->setEventHandler(handleButton);
  bc->setFeature(ButtonConfig::kFeatureRepeatPress);
  bc->setFeature(ButtonConfig::kFeatureSuppressAll);

  // finally setup the actual AceButtons
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  buttonA.init(BUTTON_A_PIN, HIGH, BUTTON_A_PIN /* will use as the ID later */);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  buttonB.init(BUTTON_B_PIN, HIGH, BUTTON_B_PIN /* will use as the ID later */);
} // end setup()


void loop() {
  loopTime = millis();


  // do stuff
  buttonA.check();
  buttonB.check();


  if (printLoopTime) {
    unsigned int now = millis();
    if (now < printLoopTime) {
      Serial.printf("internal clock rollover, try again later...\n");
    } else {
      Serial.printf("total loop time was %dms\n", now - loopTime);
    }
    // either way, reset the print flag
    printLoopTime = false;
  }
} // end loop()


// do button stuff
void handleButton(AceButton* button/* button */, uint8_t eventType, uint8_t /* buttonState */) {
  uint8_t pin = button->getPin();

  switch (eventType) {
    case AceButton::kEventReleased:
      Serial.printf("%d was released\n", pin);
      break;

    case AceButton::kEventRepeatPressed:
      Serial.printf("%d was repeat pressed (aka held?)\n", pin);
      break;

  }
}
