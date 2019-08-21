#ifndef LIBFAT_HUMAN68K_FAT_H_
#define LIBFAT_HUMAN68K_FAT_H_

#include <stdint.h>

#include <functional>
#include <vector>

namespace libFAT {
namespace Human68k {

class FAT {
 public:
  using uint12_t = uint16_t;
  enum FATType {
    FAT12,
    FAT16,
  };

  FAT(const void* buffer, size_t fat_size);
  const uint12_t GetEntry(size_t cluster);
  const uint12_t GetNextFreeEntry();
  void DebugPrint() const;

 private:
  std::vector<uint12_t> entries_;
  FATType type_;
};

}  // namespace Human68k
}  // namespace libFAT

#endif  // LIBFAT_HUMAN68K_FAT_H_
