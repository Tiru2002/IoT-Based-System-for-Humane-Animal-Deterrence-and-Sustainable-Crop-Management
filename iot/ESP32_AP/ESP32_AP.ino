#include <WiFi.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "DFRobotDFPlayerMini.h"

#define irSensor 23

DFRobotDFPlayerMini player;
TinyGPSPlus gps;
HardwareSerial mySerial(1); // GPS: TX=16, RX=17

WiFiServer server(80);
WiFiClient client;

const char* ssid = "ESP32-AP";
const char* password = "12345678";

float manualLatitude = 12.8489;
float manualLongitude = 80.1951;

char pydata, pychar;
String lastAnimal = "";
String gpsData = "";

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // GPS
  pinMode(irSensor, INPUT);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("[WiFi] AP IP address: ");
  Serial.println(IP);

  server.begin();
  Serial.println("[Server] Started");

  Serial2.begin(9600, SERIAL_8N1, 27, 26); // DFPlayer
  if (!player.begin(Serial2)) {
    Serial.println("[DFPlayer] Initialization failed!");
  } else {
    Serial.println("[DFPlayer] Ready.");
    player.volume(20);
  }
}

void loop() {
  client = server.available();
  if (client) {
    Serial.println("[Client] Connected");

    while (client.connected()) {
      while (mySerial.available()) {
        gps.encode(mySerial.read());
      }

      // Handle HTTP Request
      if (client.available()) {
        String req = client.readStringUntil('\r');
        Serial.println("[Request] " + req);

        while (client.available()) client.read(); // Flush

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();

        // Prepare GPS data
        if (gps.location.isValid()) {
          gpsData = "Latitude: " + String(gps.location.lat(), 6) +
                    " Longitude: " + String(gps.location.lng(), 6);
        } else {
          gpsData = "Latitude: " + String(manualLatitude, 6) +
                    " Longitude: " + String(manualLongitude, 6);
        }

        String response = "";
        if (lastAnimal != "") {
          response += lastAnimal + " Detected\n";
        } else {
          response += "No Animal Detected\n";
        }
        response += gpsData;

        client.println(response);
        Serial.println("[Response] Sent to client:");
        Serial.println(response);
        break;
      }

      // Trigger on IR
      if (digitalRead(irSensor) == HIGH) {
        serialEvent(); // Handles Serial input from Python
      }
    }
    client.stop();
    Serial.println("[Client] Disconnected");
  }
}

void handleAnimalDetection(const String& message, int track) {
  lastAnimal = message;
  Serial.println("[Animal] " + message);
  player.volume(25);
  player.play(track);
  delay(500);
}

void serialEvent() {
  while (Serial.available()) {
    pydata = Serial.read();

    if (pychar != pydata) {
      pychar = pydata;
      switch (pychar) {
        case 'A': handleAnimalDetection("Buffalo", 1); break;
        case 'B': handleAnimalDetection("Elephant", 2); break;
        case 'C': handleAnimalDetection("Rhino", 3); break;
        case 'D': handleAnimalDetection("Zebra", 4); break;
        case 'E': handleAnimalDetection("Cheetah", 5); break;
        case 'F': handleAnimalDetection("Fox", 6); break;
        case 'G': handleAnimalDetection("Jaguar", 7); break;
        case 'H': handleAnimalDetection("Tiger", 8); break;
        case 'I': handleAnimalDetection("Lion", 9); break;
        case 'J': handleAnimalDetection("Panda", 10); break;
      }
      pychar = 'x'; // Reset
    }
  }
}
