#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define BUZZER_PIN    15

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "ESP32-AP";
const char* password = "12345678";
const char* host = "192.168.4.1";
const int port = 80;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected!");
  display.display();
  delay(1000);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Server connection failed");
    return;
  }

  client.println("GET / HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; // Headers end
  }

  String response = client.readString();
  Serial.println("Server Response:");
  Serial.println(response);

  // Default values
  String animal = "None";
  String lat = "N/A";
  String lng = "N/A";

  int aIdx = response.indexOf("Detected");
  if (aIdx != -1) {
    int start = response.lastIndexOf('\n', aIdx - 2);
    animal = response.substring(start + 1, aIdx);
    animal.trim();
    if (animal == "No Animal") {
      digitalWrite(BUZZER_PIN, LOW);
    } else {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }

  int latIdx = response.indexOf("Latitude:");
  int lngIdx = response.indexOf("Longitude:");
  if (latIdx != -1 && lngIdx != -1) {
    lat = response.substring(latIdx + 9, lngIdx);
    lng = response.substring(lngIdx + 10);
    lat.trim();
    lng.trim();
  }

  // OLED Output
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("");
  display.println("Animal: " + animal);
  display.println("");
  display.println("");
  display.println("Latitude: " + String(lat));
  display.println("Longitude:" + String(lng));
  display.display();

  client.stop();
  delay(3000);
}
