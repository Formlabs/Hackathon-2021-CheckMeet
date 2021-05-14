#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "lib_firmware.h"

TEST_CASE( "rnd() returns 4" ) {
    REQUIRE( rnd() == 4 );
}
