#include "catch/catch.hpp"
#include "ByteManipulation.hpp"
#include <bitset>
#include <iostream>

TEST_CASE("ByteManipulation Checks")
{
    SECTION("Endian Swaps")
    {
        SECTION("Floating Point Swap")
        {
            float a = 123.456;
            float b = 151849983732479886227272230303170560.0f;
            REQUIRE(ByteManipulation::swapEndianFloat(a) == b);
        }
        SECTION("Standard Swap")
        {
            uint8_t ubyte = 0x01;
            uint8_t sbyte = 0x01;
            uint16_t u2 = 0x1A2B;
            uint16_t s2 = 0x1A2B;
            uint32_t u4 = 0x1A2B3C4D;
            uint32_t s4 = 0x1A2B3C4D;

            REQUIRE(ByteManipulation::swapEndian(ubyte) == 0x01);
            REQUIRE(ByteManipulation::swapEndian(sbyte) == 0x01);
            REQUIRE(ByteManipulation::swapEndian(u2) == 0x2B1A);
            REQUIRE(ByteManipulation::swapEndian(s2) == 0x2B1A);
            REQUIRE(ByteManipulation::swapEndian(u4) == 0x4D3C2B1A);
            REQUIRE(ByteManipulation::swapEndian(s4) == 0x4D3C2B1A);
        }
    }
    SECTION("Byte Extraction")
    {
        SECTION("Unsigned Extract")
        {
            uint8_t ubyte = 0b00010000;
            uint8_t ubyterange = 0b01010000;
            uint16_t u2 = 0b0000000100000000;
            uint16_t u2range = 0b0000000101000000;
            uint32_t u4 = 0x00001000;
            uint32_t u4range = 0x00001100;

            REQUIRE(ByteManipulation::extract8(ubyte, 4, 1) == 0b1);
            REQUIRE(ByteManipulation::extract8(ubyterange, 4, 3) == 0b101);
            REQUIRE(ByteManipulation::extract16(u2, 7, 1) == 0b1);
            REQUIRE(ByteManipulation::extract16(u2range, 7, 3) == 0b101);

        }
        SECTION("Signed Exract")
        {
            uint8_t sbyte = 0b00010000;
            uint8_t sbyterange = 0b01010000;
            uint16_t s2 = 0b0000000100000000;
            uint16_t s2range = 0b0000000101000000;
            uint32_t s4 = 0b00000000000000010000000000000000;
            uint32_t s4range = 0b00000000000001010000000000000000;

            REQUIRE(ByteManipulation::extract8Signed(sbyte, 3, 1) == 0b1);
            REQUIRE(ByteManipulation::extract8Signed(sbyterange, 1, 3) == 5);
            REQUIRE(ByteManipulation::extract16Signed(s2, 7, 1) == 0b1);
            REQUIRE(ByteManipulation::extract16Signed(s2range, 7, 3) == 0b101);
        }
    }
    SECTION("Byte Merge")
    {
        SECTION("Standard Merge")
        {
            uint8_t b1 = 0b00000001;
            uint8_t b2 = 0b00000010;
            uint8_t b3 = 0b00000100;
            uint8_t b4 = 0b00001000;
            REQUIRE(ByteManipulation::mergeBytes(b1, b2, b3, b4, 4) == 0b00000001000000100000010000001000);
            REQUIRE(ByteManipulation::mergeBytes(b1, b2, b3, b4, 3) == 0b000000010000001000000100);
        }
        SECTION("64bit Merge")
        {
            uint8_t b1 = 0x01;
            uint8_t b2 = 0x02;
            uint8_t b3 = 0x03;
            uint8_t b4 = 0x04;
            uint8_t b5 = 0x05;
            uint8_t b6 = 0x06;
            uint8_t b7 = 0x07;
            uint8_t b8 = 0x08;

            REQUIRE(ByteManipulation::mergeBytes64(b1, b2, b3, b4, b5, b6, b7, b8) == 0x0102030405060708);
        }

        SECTION("16bit Merge")
        {
            uint8_t b1 = 0b00000001;
            uint8_t b2 = 0b10000000;

            REQUIRE(ByteManipulation::mergeBytes16(b1, b2) == 0b0000000110000000);
        }
    }
}
