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
        UNSCOPED_INFO("Log: " << std::string(message.data(), message.size()));
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

TEST_CASE("Firmware handles LEDs for two clients") {
    FakeDevice device;
    std::unique_ptr<I_Firmware> firmware = make_unique<Firmware>(device);

    INFO("client #1 init");
    firmware->loopStarted(0);
    firmware->udpReceived(0, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
    firmware->loopEnded(0);
    REQUIRE(device.microphone == Color::Off);
    REQUIRE(device.webcam == Color::Off);

    INFO("client #2 init");
    firmware->loopStarted(1000);
    firmware->udpReceived(1000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"9a11f5c3-bb0f-441b-9825-9e7891bfb78c"})");
    firmware->loopEnded(1000);
    REQUIRE(device.microphone == Color::Off);
    REQUIRE(device.webcam == Color::Off);

    INFO("client #1 turns on microphone");
    firmware->loopStarted(10000);
    firmware->udpReceived(10000, R"({"version":1,"webcam":false,"microphone":true,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
    firmware->loopEnded(10000);
    REQUIRE(device.microphone == Color::On);
    REQUIRE(device.webcam == Color::Off);

    INFO("client #2 still hasn't turned on microphone");
    firmware->loopStarted(11000);
    firmware->udpReceived(11000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"9a11f5c3-bb0f-441b-9825-9e7891bfb78c"})");
    firmware->loopEnded(11000);
    REQUIRE(device.microphone == Color::On);
    REQUIRE(device.webcam == Color::Off);

    INFO("client #1 keeps microphone on");
    firmware->loopStarted(20000);
    firmware->udpReceived(20000, R"({"version":1,"webcam":false,"microphone":true,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
    firmware->loopEnded(20000);
    REQUIRE(device.microphone == Color::On);
    REQUIRE(device.webcam == Color::Off);

    INFO("client #2 turns on microphone & webcam");
    firmware->loopStarted(21000);
    firmware->udpReceived(21000, R"({"version":1,"webcam":true,"microphone":true,"senderId":"9a11f5c3-bb0f-441b-9825-9e7891bfb78c"})");
    firmware->loopEnded(21000);
    REQUIRE(device.microphone == Color::On);
    REQUIRE(device.webcam == Color::On);

    INFO("client #1 turns microphone off");
    firmware->loopStarted(30000);
    firmware->udpReceived(30000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
    firmware->loopEnded(30000);
    REQUIRE(device.microphone == Color::On);
    REQUIRE(device.webcam == Color::On);

    INFO("client #2 keeps webcam on, turns microphone off");
    firmware->loopStarted(31000);
    firmware->udpReceived(31000, R"({"version":1,"webcam":true,"microphone":false,"senderId":"9a11f5c3-bb0f-441b-9825-9e7891bfb78c"})");
    firmware->loopEnded(31000);
    REQUIRE(device.microphone == Color::Off);
    REQUIRE(device.webcam == Color::On);

    INFO("client #1 turns webcam on");
    firmware->loopStarted(40000);
    firmware->udpReceived(40000, R"({"version":1,"webcam":true,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
    firmware->loopEnded(40000);
    REQUIRE(device.microphone == Color::Off);
    REQUIRE(device.webcam == Color::On);

    INFO("client #2 keeps webcam on");
    firmware->loopStarted(41000);
    firmware->udpReceived(41000, R"({"version":1,"webcam":true,"microphone":false,"senderId":"9a11f5c3-bb0f-441b-9825-9e7891bfb78c"})");
    firmware->loopEnded(41000);
    REQUIRE(device.microphone == Color::Off);
    REQUIRE(device.webcam == Color::On);

    INFO("client #1 turns webcam off");
    firmware->loopStarted(50000);
    firmware->udpReceived(50000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
    firmware->loopEnded(50000);
    REQUIRE(device.microphone == Color::Off);
    REQUIRE(device.webcam == Color::On);

    SECTION("normal shutdown for client #2") {
        INFO("client #2 turns webcam off");
        firmware->loopStarted(51000);
        firmware->udpReceived(51000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"9a11f5c3-bb0f-441b-9825-9e7891bfb78c"})");
        firmware->loopEnded(51000);
        REQUIRE(device.microphone == Color::Off);
        REQUIRE(device.webcam == Color::Off);
    }

    SECTION("timeout for client #2") {
        INFO("client #1 keeps webcam off (9 seconds since #2 updated)");
        firmware->loopStarted(50000);
        firmware->udpReceived(50000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
        firmware->loopEnded(50000);
        REQUIRE(device.microphone == Color::Off);
        REQUIRE(device.webcam == Color::On);

        INFO("client #1 keeps webcam off (19 seconds since #2 updated)");
        firmware->loopStarted(60000);
        firmware->udpReceived(60000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
        firmware->loopEnded(60000);
        REQUIRE(device.microphone == Color::Off);
        REQUIRE(device.webcam == Color::On);

        INFO("client #1 keeps webcam off (29 seconds since #2 updated)");
        firmware->loopStarted(70000);
        firmware->udpReceived(70000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
        firmware->loopEnded(70000);
        REQUIRE(device.microphone == Color::Off);
        REQUIRE(device.webcam == Color::On);

        SECTION("timeout triggered by simple loop") {
            INFO("loop (30 seconds since #2 updated)");
            firmware->loopStarted(71000);
            firmware->loopEnded(71000);
            REQUIRE(device.microphone == Color::Off);
            REQUIRE(device.webcam == Color::On);

            INFO("loop (31 seconds since #2 updated)");
            firmware->loopStarted(72000);
            firmware->loopEnded(72000);
            REQUIRE(device.microphone == Color::Off);
            REQUIRE(device.webcam == Color::Off);
        }

        SECTION("timeout triggered by client #1") {
            INFO("client #1 keeps webcam off (39 seconds since #2 updated)");
            firmware->loopStarted(80000);
            firmware->udpReceived(80000, R"({"version":1,"webcam":false,"microphone":false,"senderId":"51000b59-b3eb-4664-a895-e824260d9050"})");
            firmware->loopEnded(80000);
            REQUIRE(device.microphone == Color::Off);
            REQUIRE(device.webcam == Color::Off);
        }
    }
}
