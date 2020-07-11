#include <AceButton.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ScoreBoardDisplay.h>

using namespace ace_button;

// WiFi credentials
const char *ssid = "cornhole";
const char *password = "first221";
const String host = "http://192.168.4.1";

// usefull for debugging how long the loop function runs
unsigned long loopTime = 0;
bool printLoopTime = false;

// for posterity?
const int LED_PIN = LED_BUILTIN;
const int LED_ON = HIGH;
const int LED_OFF = LOW;

// define what pins the buttons are on
const int BUTTON_A_PIN = D3;
const int BUTTON_B_PIN = D4;

// define what pins the scoreboard is on
const int TM1637_CLK = D1;
const int TM1637_DIO = D2;

// declare the button stuff
AceButton buttonA(BUTTON_A_PIN);
AceButton buttonB(BUTTON_B_PIN);
void handleButton(AceButton*, uint8_t, uint8_t);

// declare the scoreboard
ScoreBoardDisplay sb(TM1637_CLK, TM1637_DIO);

// declare some stuff related to updating the other esp
void updateServer();
bool updateFlag = false;
unsigned long updateTime = 0;
const unsigned long updateDelay = 2000;
HTTPClient http;


void setup() {
  Serial.begin(115200);
  Serial.println();

  // turn off the built-in LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  // how to identify buttons?
  //  Serial.printf("LED_PIN = LED_BUILTIN = %d\n", LED_PIN);
  //  Serial.printf("buttonA (%d) is attached to BUTTON_A_PIN (%d)\n", &buttonA, BUTTON_A_PIN);
  //  Serial.printf("buttonB (%d) is attached to BUTTON_B_PIN (%d)\n", &buttonB, BUTTON_B_PIN);

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

  // setup the display
  sb.setup();

  // setup the wifi stuff
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  Serial.printf("Connecting to %s", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("connected on ");
  Serial.println(WiFi.localIP());

} // end setup()


void loop() {
  loopTime = millis();


  // do stuff
  buttonA.check();
  buttonB.check();
  sb.loop();

  if (updateFlag) {
    int nowMS = millis();
    if (nowMS < updateTime) {
      // internal clock rollover, just delay it by resetting the update timer
      updateTime = 0;
    } else if (nowMS - updateTime >= updateDelay) {
      updateServer();
      updateTime = nowMS;
    }
  }

  if (printLoopTime) {
    unsigned int nowMS = millis();
    if (nowMS < printLoopTime) {
      Serial.printf("internal clock rollover, try again later...\n");
    } else {
      Serial.printf("total loop time was %dms\n", nowMS - loopTime);
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
      if (pin == BUTTON_A_PIN) sb.incrementHomeScore(); //Serial.println("Button A released");
      else if (pin == BUTTON_B_PIN) sb.incrementAwayScore(); // Serial.println("Button B released");
      else  {
        Serial.println("Unidentified button released!?");
        return;
      }
      break;

    case AceButton::kEventRepeatPressed:
      if (pin == BUTTON_A_PIN) sb.decrementHomeScore(); // Serial.println("Button A held");
      else if (pin == BUTTON_B_PIN) sb.decrementAwayScore(); // Serial.println("Button B held");
      else Serial.println("Unidentified button held!?");
      break;

    case AceButton::kEventPressed:
      // since we can't turn it off, just ignore it
      return;

    default:
      Serial.printf("Unhandled button event: %d\n", eventType);
      return;
  }
  // if we've made it this far, then update the score on the server (after a delay)
  updateTime = millis();
  updateFlag = true;
  printLoopTime = true;
}


// create an update for the server
void updateServer() {
  String url = "/setScore?scoreA=" + (String)sb.getHomeScore() + "&scoreB=" + (String)sb.getAwayScore();
  Serial.println(host + url);

  http.begin(host + url);

  int httpCode = http.GET();            //Send the request
  if (httpCode == 200) updateFlag = false;

  Serial.printf("httpCode = %d\n", httpCode);   //Print HTTP return code

  http.end();  //Close connection
  Serial.println("connection closed");
  printLoopTime = true;
} // end updateServer()
