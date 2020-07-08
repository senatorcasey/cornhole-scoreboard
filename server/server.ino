#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "cornhole"; // The name of the Wi-Fi network that will be created
const char *password = "first221";


// create a webserver on port 80
ESP8266WebServer server(80);

// function prototypes for HTTP handlers
void handleRoot();
void handleNotFound();

unsigned long dbgTime; 
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
  server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}

void loop()
{
  dbgTime = millis();
  server.handleClient();                    // Listen for HTTP requests from clients
//  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
//  delay(3000);
  if (printLoopTime) {
    Serial.printf("total loop time was %d\n", millis() - dbgTime);
    printLoopTime = false;
  }
}

void handleRoot() {
  server.send(200, "text/plain", "Hello world!");   // Send HTTP status 200 (Ok) and send some text to the browser/client
  printLoopTime = true;
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
  printLoopTime = true;
}
