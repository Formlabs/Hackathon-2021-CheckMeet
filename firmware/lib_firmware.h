#pragma once

#include <cstdio>
#include <iterator>
#include <unordered_map>

#include "stdextra.h"

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include "ArduinoJson-v6.18.0.h"

constexpr unsigned long TIMEOUT_MS = 30000;

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

    struct ClientInfo {
        Timestamp lastUpdate = 0;
        bool microphone = false;
        bool webcam = false;
    };

    using Clients = std::unordered_map<std::string, ClientInfo>;
    Clients m_Clients;

    void refreshLeds() {
        bool microphone = std::any_of(m_Clients.begin(), m_Clients.end(), [](const Clients::value_type& p) { return p.second.microphone; });
        bool webcam = std::any_of(m_Clients.begin(), m_Clients.end(), [](const Clients::value_type& p) { return p.second.webcam; });
        m_Device.setMicrophoneLeds(microphone ? Color::On : Color::Off);
        m_Device.setWebcamLeds(webcam ? Color::On : Color::Off);
    }
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
        std::string senderId;
        if (doc.containsKey("senderId")) {
            senderId = doc["senderId"].as<std::string>();
            m_Device.log(fmt("senderId %s\n", senderId.c_str()));
        }
        const auto microphone = doc["microphone"].as<bool>();
        const auto webcam = doc["webcam"].as<bool>();
        m_Device.log(fmt("microphone %s\n", microphone ? "ON" : "OFF"));
        m_Device.log(fmt("webcam %s\n", webcam ? "ON" : "OFF"));

        ClientInfo& client = m_Clients[senderId];
        client.lastUpdate = ts;
        client.microphone = microphone;
        client.webcam = webcam;
        refreshLeds();
    }

    virtual void loopStarted(Timestamp ts) override {
        size_t erased = erase_if(m_Clients, [ts](const Clients::value_type& p) { return ts - p.second.lastUpdate > TIMEOUT_MS; });
        if (erased > 0) {
            refreshLeds();
        }
    }

    virtual void loopEnded(Timestamp ts) override {
        m_Device.displayNumber(m_Counter++);
    }
};

int rnd() { return 4; }

