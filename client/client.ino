#include <ESP8266WiFi.h>

// WiFi credentials
const char *ssid = "cornhole"; // The name of the Wi-Fi network that will be created
const char *password = "first221";

// usefull for debugging how long the loop function runs
unsigned long loopTime = 0;
bool printLoopTime = false;


/* testing only */
const int buttonPin = D3;     // the number of the pushbutton pin
const int ledPin =  D4;      // the number of the LED pin


void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.printf("is D4 defined? %d\n", D4);


  /* testing only */
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  
} // end setup()


void loop() {
  loopTime = millis();


  // do stuff
  digitalWrite(ledPin, digitalRead(buttonPin));


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
