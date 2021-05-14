#pragma once

#include <cstdio>
#include <iterator>

#include "stdextra.h"
#include "ArduinoJson-v6.18.0.h"

using Timestamp = unsigned long;

enum class Color {
    On, Off, Standby
};

class I_Device {
public:
    virtual void log(StringView message) = 0;
    virtual void setMicrophoneLeds(Color color) = 0;
    virtual void setWebcamLeds(Color color) = 0;
    virtual void displayNumber(int number) = 0;
    virtual ~I_Device() = default;
};

class I_Firmware {
public:
    virtual void udpReceived(Timestamp ts, StringView incomingPacket) = 0;
    virtual void loopStarted(Timestamp ts) = 0;
    virtual void loopEnded(Timestamp ts) = 0;
    virtual ~I_Firmware() = default;
};

class Firmware : public I_Firmware {
    I_Device& m_Device;
    int m_Counter = 0;

public:
    explicit Firmware(I_Device& device) : m_Device(device) {}

    virtual void udpReceived(Timestamp ts, StringView incomingPacket) override {
        m_Device.log(fmt("UDP packet contents: %.*s\n", static_cast<int>(incomingPacket.size()), incomingPacket.data()));

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, incomingPacket.data(), incomingPacket.size());

        // Test if parsing succeeds.
        if (error) {
            m_Device.log("deserializeJson() failed: "_sv);
            m_Device.log(error.c_str());
            return;
        }

        m_Device.log(fmt("version %d\n", doc["version"].as<int>()));
        //m_Device.log(fmt("senderId %s\n", doc["senderId"].as<const char*>()));
        const auto microphone = doc["microphone"].as<bool>();
        const auto webcam = doc["webcam"].as<bool>();
        m_Device.log(fmt("microphone %s\n", microphone ? "ON" : "OFF"));
        m_Device.log(fmt("webcam %s\n", webcam ? "ON" : "OFF"));
        m_Device.setMicrophoneLeds(microphone ? Color::On : Color::Off);
        m_Device.setWebcamLeds(webcam ? Color::On : Color::Off);
    }
    virtual void loopStarted(Timestamp ts) override {

    }
    virtual void loopEnded(Timestamp ts) override {
        m_Device.displayNumber(m_Counter++);
    }
};

int rnd() { return 4; }

