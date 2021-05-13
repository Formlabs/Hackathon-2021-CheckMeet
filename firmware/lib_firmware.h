#pragma once

#include <cstdio>
#include <iterator>
#include <unordered_map>
#include <tuple>

#include "stdextra.h"

#define ARDUINOJSON_ENABLE_STD_STRING 1
#include "ArduinoJson-v6.18.0.h"

using MonotonicTime = unsigned long;

struct Timestamp {
    MonotonicTime monotonic;
    time_t clock;

    /* implicit */ Timestamp(MonotonicTime mt, time_t c = 0) : monotonic{mt}, clock{c} {}
};

constexpr MonotonicTime DEFAULT_CLIENT_TIMEOUT_MS = 30000;

enum class Color {
    On, Off, Standby, Initializing
};

class I_Device {
public:
    virtual void log(StringView message) = 0;
    virtual void setMicrophoneLeds(Color color) = 0;
    virtual void setWebcamLeds(Color color) = 0;
    virtual void displayTime(int minutes, int seconds) = 0;
    virtual void clearDisplay() = 0;
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
    time_t m_LastClock = 0;

    struct ClientInfo {
        MonotonicTime lastUpdate = 0;
        bool microphone = false;
        bool webcam = false;
        time_t countDownTarget = 0;
    };

    using Clients = std::unordered_map<std::string, ClientInfo>;
    Clients m_Clients;
    const MonotonicTime m_ClientTimeout_ms;

    void refreshLeds() {
        if (m_Clients.empty()) {
            m_Device.setMicrophoneLeds(Color::Standby);
            m_Device.setWebcamLeds(Color::Standby);
            return;
        }
        bool microphone = std::any_of(m_Clients.begin(), m_Clients.end(), [](const Clients::value_type& p) { return p.second.microphone; });
        bool webcam = std::any_of(m_Clients.begin(), m_Clients.end(), [](const Clients::value_type& p) { return p.second.webcam; });
        m_Device.setMicrophoneLeds(microphone ? Color::On : Color::Off);
        m_Device.setWebcamLeds(webcam ? Color::On : Color::Off);
    }

    void refreshDisplay(Timestamp ts) {
        const auto isCounting = [&](const ClientInfo& client) {
            return client.countDownTarget != 0 && ts.clock < client.countDownTarget;
        };
        const auto isCloser = [&](const Clients::value_type& lhs, const Clients::value_type& rhs) {
            const auto l = lhs.second.countDownTarget;
            const auto lNotCounting = !isCounting(lhs.second);
            const auto r = rhs.second.countDownTarget;
            const auto rNotCounting = !isCounting(rhs.second);
            return std::tie(lNotCounting, l) < std::tie(rNotCounting, r);
        };
        const auto min = std::min_element(m_Clients.begin(), m_Clients.end(), isCloser);
        if (min != m_Clients.end() && isCounting(min->second)) {
            showSeconds(min->second.countDownTarget - ts.clock);
        } else {
            showSeconds(0);
        }
    }

    void showSeconds(int seconds) {
        const auto maxDisplayed = 99 * 60 + 59;
        seconds = clamp(seconds, 0, maxDisplayed);
        if (seconds) {
            m_Device.displayTime(seconds / 60, seconds % 60);
        } else {
            m_Device.clearDisplay();
        }
    }

public:
    explicit Firmware(I_Device &device, MonotonicTime clientTimeout_ms = DEFAULT_CLIENT_TIMEOUT_MS)
        : m_Device(device)
        , m_ClientTimeout_ms(clientTimeout_ms)
    {
          m_Device.setMicrophoneLeds(Color::Initializing);
          m_Device.setWebcamLeds(Color::Initializing);
    }

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
        client.lastUpdate = ts.monotonic;
        client.microphone = microphone;
        client.webcam = webcam;
        refreshLeds();

        if (doc.containsKey("countDownTarget")) {
            client.countDownTarget = doc["countDownTarget"].as<int>();
            m_Device.log(fmt("countDownTarget %ld  %s\n", client.countDownTarget, ctime(&client.countDownTarget)));
            {
                m_Device.log(fmt("time(nullptr)   %ld  %s\n", ts.clock, ctime(&ts.clock)));
            }
        } else {
            client.countDownTarget = 0;
        }
        refreshDisplay(ts);
    }

    virtual void loopStarted(Timestamp ts) override {
        erase_if(m_Clients, [ts, this](const Clients::value_type& p) {
            const auto result = ts.monotonic - p.second.lastUpdate > m_ClientTimeout_ms;
            if (result) {
                m_Device.log(fmt("Client \"%s\" times out\n", p.first.c_str()));
            }
            return result;
        });
        refreshLeds();
    }

    virtual void loopEnded(Timestamp ts) override {
        if (ts.clock != m_LastClock) {
            m_LastClock = ts.clock;
            refreshDisplay(ts);
        }
    }
};

int rnd() { return 4; }
