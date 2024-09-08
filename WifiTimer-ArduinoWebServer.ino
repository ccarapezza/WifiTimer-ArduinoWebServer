// Import required libraries
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Chori-NET"; // Replace with your own SSID
const char* password = "00434081431"; // Replace with your own password

const int ledPin = 2; // Parametrize the LED pin
bool ledState = false; // false means OFF, true means ON

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var) {
  Serial.println("Setting value...");
  Serial.print("[");
  Serial.print(var);
  Serial.print("]");
  if (var == "GPIO_STATE") {
    if (ledState) {
      return "ON";
    } else {
      return "OFF";
    }
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Ensure LED is initially off

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Test...");
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to JS script
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/script.js", "text/javascript");
  });
  
  server.on("/DSEG7Modern-BoldItalic.woff", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/DSEG7Modern-BoldItalic.woff", "font/woff");
  });

  // Route to set GPIO to HIGH
  server.on("/led2on", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Turning on...");
    digitalWrite(ledPin, LOW);    
    ledState = true;
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to LOW
  server.on("/led2off", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Turning off...");
    digitalWrite(ledPin, HIGH);
    ledState = false;
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  server.on("/api/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Turning off...");
    digitalWrite(ledPin, HIGH);
    request->send(200, "text/plain", "OFF");
  });

  server.on("/api/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Turning on...");
    digitalWrite(ledPin, LOW);
    request->send(200, "text/plain", "ON");
  });

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    int state = digitalRead(ledPin); // Lee el estado del pin
    Serial.print("Estado del pin: ");
    Serial.println(state);
    request->send(200, "text/plain", state == HIGH ? "OFF" : "ON");
  });

  // Start server
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
  server.begin();
}

void loop() {

}