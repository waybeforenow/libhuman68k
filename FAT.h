#ifndef LIBFAT_HUMAN68K_FAT_H_
#define LIBFAT_HUMAN68K_FAT_H_

#include <stdint.h>

#include <functional>
#include <vector>

namespace libFAT {
namespace Human68k {

/* @brief Internal representation of a FAT
 *
 * The FAT class represents a File Allocation Table from the disk image
 * internally. It provides accessors to traverse the FAT and modifiers to insert
 * new entries into the FAT.
 */
class FAT {
 public:
  using uint12_t = uint16_t;
  enum FATType {
    FAT12,
    FAT16,
  };

  FAT(FATType type, const void* buffer, size_t fat_size);
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
