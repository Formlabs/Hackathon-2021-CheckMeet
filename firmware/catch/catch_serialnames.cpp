#include "catch.hpp"

#include "serialnames.h"

TEST_CASE( "computeNameForId() works" ) {
    REQUIRE( computeNameForId(0xa451be) == "TealFrenchCuteLion");
    REQUIRE( computeNameForId(0x000000) == "AmberAkanAbleAnt");
    REQUIRE( computeNameForId(0xffffff) == "WhiteZuluWetYak");
    REQUIRE( computeNameForId(0x1000000) == "WhiteZuluWetYak");
    REQUIRE( computeNameForId(0x00001f) == "WhiteAkanAbleAnt");
    REQUIRE( computeNameForId(0x000020) == "AmberAmharicAbleAnt");
}
