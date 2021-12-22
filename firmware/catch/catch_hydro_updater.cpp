#include "catch.hpp"

#include "hydro_updater.h"
#include "stdextra.h"

TEST_CASE( "hydro signature verification works if used like in Updater.cpp" ) {
    static const uint8_t signature[64] = {
        0x54, 0xf3, 0xf1, 0x45, 0x9e, 0x36, 0x9c, 0xe9,
        0x85, 0x88, 0x6a, 0xfc, 0x51, 0xb1, 0xe3, 0x2f,
        0x86, 0x30, 0x8c, 0xd8, 0x2b, 0xf5, 0x3e, 0x6b,
        0x51, 0xc1, 0x7e, 0x3c, 0x72, 0x09, 0x16, 0x32,
        0xa3, 0x5e, 0x44, 0x3b, 0x76, 0xb8, 0xb1, 0xde,
        0xa9, 0x07, 0x64, 0x99, 0xea, 0x5a, 0x18, 0xa4,
        0xd9, 0xa5, 0x5f, 0xb8, 0x5a, 0x49, 0xc9, 0x97,
        0xd1, 0xfd, 0x50, 0x4b, 0xea, 0xc4, 0x21, 0x01,
    };
    static const uint8_t pubkey[32] = {
        0xf9, 0x6d, 0x67, 0x88, 0x82, 0xeb, 0x48, 0xe8,
        0xf4, 0x6d, 0xc5, 0xbd, 0xb6, 0xeb, 0xe4, 0x0e,
        0x78, 0x3c, 0xff, 0xf5, 0x5e, 0x48, 0xfe, 0xd6,
        0xd7, 0xf4, 0x50, 0xc0, 0xae, 0xf3, 0x83, 0x7f,
    };
    static const uint8_t message[5] = { 'h', 'e', 'l', 'l', 'o' };

    auto hashAndVerify = make_unique<HydroUpdaterHashAndVerify>(Span<const uint8_t>{pubkey, sizeof(pubkey)});
    UpdaterHashClass *hash = hashAndVerify.get();
    UpdaterVerifyClass *verify = hashAndVerify.get();

    REQUIRE(verify->length() == sizeof(signature));

    SECTION( "verify succeeds if everything is the expected" ) {
        hash->begin();
        hash->add(message, sizeof(message));
        hash->end();
        REQUIRE(verify->verify(hash, signature, sizeof(signature)));
    }

    SECTION( "verify fails if some bit of the signature is flipped" ) {
        uint8_t signature2[64];
        std::copy(std::begin(signature), std::end(signature), std::begin(signature2));
        signature2[22] ^= 0x40;
        hash->begin();
        hash->add(message, sizeof(message));
        hash->end();
        REQUIRE(!verify->verify(hash, signature2, sizeof(signature2)));
    }

    SECTION( "verify fails if some bit of the message is flipped" ) {
        uint8_t message2[sizeof(message)];
        std::copy(std::begin(message), std::end(message), std::begin(message2));
        message2[3] ^= 0x20;
        hash->begin();
        hash->add(message2, sizeof(message2));
        hash->end();
        REQUIRE(!verify->verify(hash, signature, sizeof(signature)));
    }
}
