#include "catch.hpp"

#include "stdextra.h"

TEST_CASE( "fmt() works" ) {
    REQUIRE( fmt("Hello, %s!", "World") == "Hello, World!" );
}
