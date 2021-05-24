#include <WiFiManager.h>
#include <WiFiUdp.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#include <TM1637Display.h>

#include "lib_firmware.h"

class Device : public I_Device {
  static constexpr int NUM_LEDS = 6;
  CRGB leds[NUM_LEDS];
  static constexpr auto PIN_LEDS = D2;

  static constexpr int DISPLAY_CLK = D6;
  static constexpr int DISPLAY_DIO = D5;
  TM1637Display display{DISPLAY_CLK, DISPLAY_DIO};

  CRGB decode(Color color) {
    switch(color) {
      case Color::On: return CRGB::Red;
      case Color::Off: return CRGB::Green;
      case Color::Standby: return CRGB::Black;
      case Color::Initializing: return CRGB::Yellow;
      default: return CRGB::Black;
    }
  }

  public:
    Device() {
      Serial.begin(74880);
      FastLED.addLeds<NEOPIXEL, PIN_LEDS>(leds, NUM_LEDS);
      display.setBrightness(0x0a); //set the diplay to maximum brightness
    }

    virtual void log(StringView message) override {
      Serial.printf("%.*s", static_cast<int>(message.size()), message.data());
    }

    virtual void setMicrophoneLeds(Color color) override {
      leds[0] = leds[1] = leds[2] = decode(color);
      FastLED.show();
    }

    virtual void setWebcamLeds(Color color) override {
      leds[3] = leds[4] = leds[5] = decode(color);
      FastLED.show();
    }

    virtual void displayNumber(int number) override {
      if (number) {
        display.showNumberDec(number);
      } else {
        display.clear();
      }
    }
};

WiFiUDP Udp;
static const uint16_t localUdpPort = 26999;

constexpr auto PIN_BUTTON = D3;
static bool button = true;

std::unique_ptr<I_Device> device;
std::unique_ptr<I_Firmware> firmware;

void setup() {
  device = make_unique<Device>();
  firmware = make_unique<Firmware>(*device);

  WiFi.mode(WIFI_STA);
  if (WiFiManager().autoConnect(fmt("CheckMeet_%06X", ESP.getChipId()).c_str())) {
    Serial.println("Connected \\o/");
  } else {
    Serial.println("Failed to connect :(");
  }
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
}

void loop() {
  Timestamp now = millis();

  firmware->loopStarted(now);

  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    char incomingPacket[255];
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    firmware->udpReceived(now, StringView(incomingPacket, len));

    // send back a reply, to the IP address and port we got the packet from
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(replyPacket);
    //Udp.endPacket();
  }

  {
    const auto newButton = digitalRead(PIN_BUTTON);
    if (newButton != button) {
      button = newButton;
      Serial.printf("button goes %d\n", button);
    }
  }
  firmware->loopEnded(now);
}
