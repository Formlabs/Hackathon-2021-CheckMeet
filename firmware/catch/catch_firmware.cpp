#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "lib_firmware.h"

TEST_CASE( "rnd() returns 4" ) {
    REQUIRE( rnd() == 4 );
}

class FakeDevice : public I_Device {
public:
    std::string console;
    Color microphone = Color::Standby;
    Color webcam = Color::Standby;
    int display = 0;

    virtual void log(StringView message) override {
        console += std::string(message.data(), message.size());
    }

    virtual void setMicrophoneLeds(Color color) override {
        microphone = color;
    }

    virtual void setWebcamLeds(Color color) override {
        webcam = color;
    }

    virtual void displayNumber(int number) override {
        display = number;
    }
};

TEST_CASE("Firmware handles LEDs for one client") {
    FakeDevice device;
    std::unique_ptr<I_Firmware> firmware = make_unique<Firmware>(device);

    SECTION("one message") {
        SECTION("microphone off, webcam off") {
            firmware->loopStarted(0);
            firmware->udpReceived(0, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
            firmware->loopEnded(0);

            REQUIRE(device.microphone == Color::Off);
            REQUIRE(device.webcam == Color::Off);
        }
        SECTION("microphone off, webcam on") {
            firmware->loopStarted(0);
            firmware->udpReceived(0, R"({"version":1,"webcam":true,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
            firmware->loopEnded(0);

            REQUIRE(device.microphone == Color::Off);
            REQUIRE(device.webcam == Color::On);
        }
        SECTION("microphone on, webcam off") {
            firmware->loopStarted(0);
            firmware->udpReceived(0, R"({"version":1,"webcam":false,"microphone":true,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
            firmware->loopEnded(0);

            REQUIRE(device.microphone == Color::On);
            REQUIRE(device.webcam == Color::Off);
        }
        SECTION("microphone on, webcam on") {
            firmware->loopStarted(0);
            firmware->udpReceived(0, R"({"version":1,"webcam":true,"microphone":true,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
            firmware->loopEnded(0);

            REQUIRE(device.microphone == Color::On);
            REQUIRE(device.webcam == Color::On);
        }
    }

    SECTION("two messages") {
        firmware->loopStarted(0);
        firmware->udpReceived(0, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
        firmware->loopEnded(0);

        REQUIRE(device.microphone == Color::Off);
        REQUIRE(device.webcam == Color::Off);

        SECTION("microphone turns on") {
            firmware->loopStarted(10000);
            firmware->udpReceived(10000, R"({"version":1,"webcam":false,"microphone":true,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
            firmware->loopEnded(10000);

            REQUIRE(device.microphone == Color::On);
            REQUIRE(device.webcam == Color::Off);
        }

        SECTION("webcam turns on") {
            firmware->loopStarted(10000);
            firmware->udpReceived(10000, R"({"version":1,"webcam":true,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
            firmware->loopEnded(10000);

            REQUIRE(device.microphone == Color::Off);
            REQUIRE(device.webcam == Color::On);
        }

        SECTION("both turn on") {
            firmware->loopStarted(10000);
            firmware->udpReceived(10000, R"({"version":1,"webcam":true,"microphone":true,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
            firmware->loopEnded(10000);

            REQUIRE(device.microphone == Color::On);
            REQUIRE(device.webcam == Color::On);
        }
    }

    SECTION("multiple messages") {
        firmware->loopStarted(0);
        firmware->udpReceived(0, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
        firmware->loopEnded(0);

        REQUIRE(device.microphone == Color::Off);
        REQUIRE(device.webcam == Color::Off);

        firmware->loopStarted(10000);
        firmware->udpReceived(10000, R"({"version":1,"webcam":false,"microphone":true,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
        firmware->loopEnded(10000);

        REQUIRE(device.microphone == Color::On);
        REQUIRE(device.webcam == Color::Off);

        firmware->loopStarted(20000);
        firmware->udpReceived(20000, R"({"version":1,"webcam":true,"microphone":true,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
        firmware->loopEnded(20000);

        REQUIRE(device.microphone == Color::On);
        REQUIRE(device.webcam == Color::On);

        firmware->loopStarted(30000);
        firmware->udpReceived(30000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
        firmware->loopEnded(30000);

        REQUIRE(device.microphone == Color::Off);
        REQUIRE(device.webcam == Color::Off);
    }
}