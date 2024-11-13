#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define BUTTON_PIN 5 // D1 pin connected to button (GPIO5)
#define LED_PIN 2    // Built-in LED (GPIO2)

const char* ssid = "nelson";
const char* password = "1234567890";
const char* apiEndpoint = "http://192.168.137.1:3000/submit"; // Your server IP

WiFiClient wifiClient; // Create a WiFiClient object

// Responder 2 info
const char* responderName = "BARA MECHIA";
const char* responderId = "ESP8266_3"; // Unique ID for Responder 2

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  digitalWrite(LED_PIN, HIGH); // Indicate WiFi connection
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection
      HTTPClient http;
      
      // Send the button press event
      String payload = "{\"id\":\"" + String(responderId) + "\",\"status\":\"button_pressed\",\"name\":\"" + String(responderName) + "\"}";

      http.begin(wifiClient, apiEndpoint);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(payload);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.println("Response: " + response);
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(http.errorToString(httpResponseCode).c_str()); // Print error
      }

      http.end();
    } else {
      Serial.println("WiFi not connected");
    }

    delay(500); // Debounce delay
  }
}
