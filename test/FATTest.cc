#include "../FAT.h"

#define CATCH_CONFIG_MAIN
#include "lib/catch.hpp"

TEST_CASE("Reads FAT12 correctly", "[fat]") {
  uint8_t fake_fat[6] = {0x76, 0x32, 0x54, 0x10, 0x32, 0x98};

  libFAT::Human68k::FAT fat(FAT12, fake_fat, 6);
  REQUIRE(fat.GetEntry(0) == 0x276);
  REQUIRE(fat.GetEntry(1) == 0x543);
  REQUIRE(fat.GetEntry(2) == 0x210);
  REQUIRE(fat.GetEntry(3) == 0x983);
  REQUIRE(fat.GetNextFreeEntry() == 0x000);
}
