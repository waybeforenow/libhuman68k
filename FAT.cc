#include "FAT.h"

#include <iostream>
#include <sstream>

#include "lib/easyloggingpp/easylogging++.h"

namespace libFAT {
namespace Human68k {

FAT::FAT(const void* buffer, size_t fat_size) {
  for (int i = 0; i < fat_size; i++) {
    const size_t offset = 3 * i / 2;
    uint12_t value;
    if (i % 2 == 0) {
      value = (((uint8_t*)buffer)[offset + 1] & 0x0f) << 8;
      value |= ((uint8_t*)buffer)[offset];
    } else {
      value = (((uint8_t*)buffer)[offset] & 0xf0) >> 4;
      value |= ((uint8_t*)buffer)[offset + 1] << 4;
    }
    entries_.push_back(value);
  }
}

const FAT::uint12_t FAT::GetEntry(size_t cluster) {
  try {
    return entries_.at(cluster);
  } catch (const std::out_of_range& oob) {
    return 0x000;
  }
}

const FAT::uint12_t FAT::GetNextFreeEntry() {
  for (const auto& entry : entries_) {
    if (entry == 0x000) {
      return entry;
    }
  }

  return 0x000;
}

void FAT::DebugPrint() const {
  LOG(INFO) << "FAT contents:";
  std::string line;
  for (int i = 0; i < entries_.size(); i++) {
    if (i % 16 == 0) {
      LOG(INFO) << line;
      line = "";
    }
    char buf[512];
    sprintf(buf, "%03x ", entries_[i]);
    line.append(buf);
  }
}

}  // namespace Human68k
}  // namespace libFAT
