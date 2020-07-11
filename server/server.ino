#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Jonathan is thehouse!! */
const char *ssid = "cornhole"; // The name of the Wi-Fi network that will be created
const char *password = "first221";


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
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
} // end setup()

void loop()
{
  loopTime = millis();
  
  server.handleClient();
  
  if (printLoopTime) {
    Serial.printf("total loop time was %dms\n", millis() - loopTime);
    printLoopTime = false;
  }
} // end loop()


// Send HTTP status 200 (Ok) and send some text to the browser/client
void handleRoot() {
  server.send(200, "text/plain", "Hello world!");
  printLoopTime = true;
}


// Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
  printLoopTime = true;
}
