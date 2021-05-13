#include <WiFiManager.h>
#include <WiFiUdp.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#include <TM1637Display.h>

#include "ArduinoJson-v6.18.0.h"

WiFiUDP Udp;
static const uint16_t localUdpPort = 26999;

constexpr int NUM_LEDS = 6;
CRGB leds[NUM_LEDS];
constexpr auto PIN_LEDS = D2;

const int DISPLAY_CLK = D6;
const int DISPLAY_DIO = D5;

int numCounter = 0;
 
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

constexpr auto PIN_BUTTON = D3;
static bool button = true;

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
  FastLED.addLeds<NEOPIXEL, PIN_LEDS>(leds, NUM_LEDS);
  display.setBrightness(0x0a); //set the diplay to maximum brightness
  pinMode(PIN_BUTTON, INPUT_PULLUP);
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
    const auto microphone = doc["microphone"].as<bool>();
    const auto webcam = doc["webcam"].as<bool>();
    Serial.printf("microphone %s\n", microphone ? "ON" : "OFF");
    Serial.printf("webcam %s\n", webcam ? "ON" : "OFF");
    leds[0] = leds[1] = leds[2] = microphone ? CRGB::Red : CRGB::Green;
    leds[3] = leds[4] = leds[5] = webcam ? CRGB::Red : CRGB::Green;
    FastLED.show();

    // send back a reply, to the IP address and port we got the packet from
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(replyPacket);
    //Udp.endPacket();
  }
  display.showNumberDec(numCounter++); //Display the numCounter value;
  {
    const auto newButton = digitalRead(PIN_BUTTON);
    if (newButton != button) {
      button = newButton;
      Serial.printf("button goes %d\n", button);
    }
  }
}
