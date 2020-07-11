#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include "libraries\TM1637Display\TM1637Display.h"
//#include "libraries\ScoreBoardDisplay\ScoreBoardDisplay.h"
#include "TM1637Display.h"
#include "ScoreBoardDisplay.h"

const char *ssid = "cornhole"; // The name of the Wi-Fi network that will be created
const char *password = "first221";

// declare the scoreboard
const int TM1637_CLK = D1;
const int TM1637_DIO = D2;
ScoreBoardDisplay sb(TM1637_CLK, TM1637_DIO);

// create a webserver on port 80
ESP8266WebServer server(80);

// function prototypes for HTTP handlers
void handleRoot();
void handleNotFound();

unsigned long loopTime = 0; 
bool printLoopTime = false;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // start a wifi access point for the other esp8266
  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP(ssid, password);
  Serial.println(result ? "Ready" : "Failed");

  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());

  // start the webserver
  server.on("/", handleRoot);
  server.on("/setScore", handleScore);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  // setup the display
  sb.setup();
} // end setup()

void loop()
{
  loopTime = millis();
  
  server.handleClient();
  sb.loop();
  
  if (printLoopTime) {
    Serial.printf("total loop time was %dms\n", millis() - loopTime);
    printLoopTime = false;
  }
} // end loop()

// scoreA and scoreB parameters are required for score to be updated
// score parameter range is 0 - 99
void handleScore() {
  int scoreA = 0;
  int scoreB = 0;
  String outputParams = "";

  //loop through parameter list and find "scoreA" and "scoreB" parameters
  for (int i = 0; i < server.args(); i++) {
    String paramName = server.argName(i);
    String paramValue = server.arg(i);

    outputParams += paramName + ":" + paramValue + "; "; //build string with all parameters

    if (paramName.equals("scoreA")) { // SCORE A
      scoreA = validateScore(paramValue);
    }
    else if (paramName.equals("scoreB")) {  // SCORE B
      scoreB = validateScore(paramValue);
    }
  }

  Serial.println(outputParams);
  if(scoreA < 100 && scoreB < 100) {
    sb.setHomeScore(scoreB);
    sb.setAwayScore(scoreA);
    serverSend("Hero: " + (String)scoreB + ", Villain:" + (String)scoreA);
  }
  else { //invalid scores, send bad request back to client
    server.send(400, "text/plain", "Bad request");
  }
}

//check if score value is a number and is with the range of 0-99
//if not valid, return 100
int validateScore(String scoreParamValue)
{
  int score = 100;
  if(isNumeric(scoreParamValue)) {
    int intScore = scoreParamValue.toInt();
    if(intScore >= 0 && intScore <= 99) {
      score = intScore;
    }
  }
  return score;
}

// Send HTTP status 200 (Ok) and send some text to the browser/client
void handleRoot() 
{
  serverSend("Hello world!");
}

// Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
void handleNotFound() 
{
  serverSend("404: Not found");
}

void serverSend(String message)
{
  server.send(200, "text/plain", message);
  printLoopTime = true;
}

//check if an entire string is numeric
boolean isNumeric(String str)
{
    unsigned int stringLength = str.length();
 
    if (stringLength == 0) {
        return false;
    }
 
    boolean seenDecimal = false;
 
    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str.charAt(i))) {
            continue;
        }
 
        if (str.charAt(i) == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}
