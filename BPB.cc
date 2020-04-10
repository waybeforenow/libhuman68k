#include "BPB.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/easyloggingpp/easylogging++.h"

namespace libFAT {
namespace Human68k {
namespace BPB {

Standard::Standard(const char* structure) {
  void* tmp_structure = malloc(0x1b);
  memcpy(tmp_structure, structure, 0x1b);
  structure_ = tmp_structure;
}

Standard::~Standard() {
  void* tmp_structure = (void*)structure_;
  free(tmp_structure);
}

const char* Standard::BootInstruction() const {
  return (const char*)structure_;
}

const char* Standard::OEMString() const {
  return (const char*)(uint8_t*)structure_ + 0x03;
}

uint16_t Standard::BytesPerSector() const {
  uint8_t b1 = *((uint8_t*)structure_ + 0x0b),
          b2 = *((uint8_t*)structure_ + 0x0c);
  uint16_t bytes_per_sector = (b2 << 8) | b1;
  return bytes_per_sector;
}

uint8_t Standard::SectorsPerCluster() const {
  uint8_t sectors_per_cluster = *((uint8_t*)structure_ + 0x0d);
  return sectors_per_cluster;
}

uint8_t Standard::FATRegionCount() const {
  uint8_t fat_region_count = *((uint8_t*)structure_ + 0x0e);
  return fat_region_count;
}

uint16_t Standard::ReservedSectorCount() const {
  uint8_t b1 = *((uint8_t*)structure_ + 0x0f),
          b2 = *((uint8_t*)structure_ + 0x10);
  uint16_t reserved_sector_count = (b1 << 8) | b2;
  return ++reserved_sector_count;
}

uint16_t Standard::RootDirectoryEntries() const {
  uint8_t b1 = *((uint8_t*)structure_ + 0x11),
          b2 = *((uint8_t*)structure_ + 0x12);
  uint16_t root_directory_entries = ((b2 << 8) | b1) >> 5;
  return root_directory_entries;
}

uint8_t Standard::MediaByte() const {
  uint8_t media_byte = *((uint8_t*)structure_ + 0x15);
  return media_byte;
}

uint8_t Standard::SectorsPerFAT() const {
  uint8_t sectors_per_fat = *((uint8_t*)structure_ + 0x16);
  return sectors_per_fat;
}

uint64_t Standard::SectorCount() const {
  uint64_t sector_count;
  if (*((uint8_t*)structure_ + 0x14) == 0 &&
      *((uint8_t*)structure_ + 0x13) == 0) {
    uint8_t b1 = *((uint8_t*)structure_ + 0x1a),
            b2 = *((uint8_t*)structure_ + 0x19),
            b3 = *((uint8_t*)structure_ + 0x18),
            b4 = *((uint8_t*)structure_ + 0x17);
    sector_count = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
  } else {
    uint8_t b1 = *((uint8_t*)structure_ + 0x14),
            b2 = *((uint8_t*)structure_ + 0x13);
    sector_count = (b1 << 8) | b2;
  }

  return sector_count;
}

Extended::Extended(const char* structure) {
  void* tmp_structure = malloc(0x22);
  memcpy(tmp_structure, structure, 0x22);
  structure_ = tmp_structure;
}

Extended::~Extended() {
  void* tmp_structure = (void*)structure_;
  free(tmp_structure);
}

const char* Extended::BootInstruction() const {
  return (const char*)structure_;
}

const char* Extended::OEMString() const {
  return (const char*)(uint8_t*)structure_ + 0x03;
}

uint16_t Extended::BytesPerSector() const {
  uint8_t b1 = *((uint8_t*)structure_ + 0x12),
          b2 = *((uint8_t*)structure_ + 0x13);
  uint16_t bytes_per_sector = (b1 << 8) | b2;
  return bytes_per_sector;
}

uint8_t Extended::SectorsPerCluster() const {
  uint8_t sectors_per_cluster = *((uint8_t*)structure_ + 0x14);
  return sectors_per_cluster;
}

uint8_t Extended::FATRegionCount() const {
  uint8_t fat_region_count = *((uint8_t*)structure_ + 0x15);
  return fat_region_count;
}

uint16_t Extended::ReservedSectorCount() const {
  uint8_t b1 = *((uint8_t*)structure_ + 0x16),
          b2 = *((uint8_t*)structure_ + 0x17);
  uint16_t reserved_sector_count = (b1 << 8) | b2;
  return reserved_sector_count;
}

uint16_t Extended::RootDirectoryEntries() const {
  uint8_t b1 = *((uint8_t*)structure_ + 0x18),
          b2 = *((uint8_t*)structure_ + 0x19);
  uint16_t root_directory_entries = ((b1 << 8) | b2) >> 5;
  return root_directory_entries;
}

uint8_t Extended::MediaByte() const {
  uint8_t media_byte = *((uint8_t*)structure_ + 0x1c);
  return media_byte;
}

uint8_t Extended::SectorsPerFAT() const {
  uint8_t sectors_per_fat = *((uint8_t*)structure_ + 0x1d);
  return sectors_per_fat;
}

uint64_t Extended::SectorCount() const {
  uint64_t sector_count;
  if (*((uint8_t*)structure_ + 0x1a) == 0 &&
      *((uint8_t*)structure_ + 0x1b) == 0) {
    uint8_t b1 = *((uint8_t*)structure_ + 0x1e),
            b2 = *((uint8_t*)structure_ + 0x1f),
            b3 = *((uint8_t*)structure_ + 0x20),
            b4 = *((uint8_t*)structure_ + 0x21);
    sector_count = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
  } else {
    uint8_t b1 = *((uint8_t*)structure_ + 0x1a),
            b2 = *((uint8_t*)structure_ + 0x1b);
    sector_count = (b1 << 8) | b2;
  }

  return sector_count;
}

void Base::DebugPrint() const {
  printf("Boot instruction:");
  const char* boot_instruction = BootInstruction();
  for (int i = 0; i < 3; i++) {
    printf("%02hhx", boot_instruction[i]);
  }
  printf("\n");
  printf("OEM string:");
  const char* oem_string = OEMString();
  for (int i = 0; i < 15; i++) {
    printf("%c", oem_string[i]);
  }
  printf("\n");
  printf("Bytes per sector: %u\n", BytesPerSector());
  printf("Sectors per cluster: %u\n", SectorsPerCluster());
  printf("FAT region count: %u\n", FATRegionCount());
  printf("Reserved sector count: %u\n", ReservedSectorCount());
  printf("Root directory entries: %u\n", RootDirectoryEntries());
  printf("Sector count: %lu\n", SectorCount());
  printf("Media byte: %hhx\n", MediaByte());
  printf("Sectors per FAT: %u\n", SectorsPerFAT());
}

}  // namespace BPB
}  // namespace Human68k
}  // namespace libFAT

