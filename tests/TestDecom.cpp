#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include "ByteManipulation.hpp"


TEST_CASE("Endian Swap Float") {
    float a = 123.456;
    std::cerr << ByteManipulation::swapEndianFloat(a);
    // REQUIRE( Factorial(10) == 3628800 );
}
