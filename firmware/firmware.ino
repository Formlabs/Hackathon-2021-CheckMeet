#include <WiFiManager.h>
#include <WiFiUdp.h>

#include "ArduinoJson-v6.18.0.h"

WiFiUDP Udp;
static const uint16_t localUdpPort = 26999;

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(74880);
  if (WiFiManager().autoConnect()) {
    Serial.println("Connected \\o/");
  } else {
    Serial.println("Failed to connect :(");
  }
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    char incomingPacket[255];
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, incomingPacket);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    Serial.printf("version %d\n", doc["version"].as<int>());
    //Serial.printf("senderId %s\n", doc["senderId"].as<const char*>());
    Serial.printf("microphone %s\n", doc["microphone"].as<bool>() ? "ON" : "OFF");
    Serial.printf("webcam %s\n", doc["webcam"].as<bool>() ? "ON" : "OFF");

    // send back a reply, to the IP address and port we got the packet from
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(replyPacket);
    //Udp.endPacket();
  }
}
