#ifndef LIBFAT_HUMAN68K_BPB_H_
#define LIBFAT_HUMAN68K_BPB_H_

#include <stdint.h>

namespace libFAT {
namespace Human68k {

/* @brief Internal representation of a BIOS Parameter Block
 *
 * A BPB (BIOS Parameter Block) contains some key pieces of metadata about a
 * disk or partition and needs to be parsed and understood before the FAT can be
 * read.
 *
 * There are 2 types of BPB that Human68k uses: see
 * http://paste.vampi.tech/5c18424a5b73f for a comparison.
 *
 * libFAT::Human68k::BPB provides classes for parsing a BPB.
 */
namespace BPB {

class Base {
 public:
  virtual ~Base() = default;

  // Implementation must provide its own OEM string, which can be 8 or 15
  // characters depending on the BPB type.
  virtual const char* OEMString() const = 0;
  virtual const char* BootInstruction() const = 0;
  virtual uint16_t BytesPerSector() const = 0;
  virtual uint8_t SectorsPerCluster() const = 0;
  virtual uint8_t FATRegionCount() const = 0;
  virtual uint16_t ReservedSectorCount() const = 0;
  virtual uint16_t RootDirectoryEntries() const = 0;
  virtual uint64_t SectorCount() const = 0;
  virtual uint8_t MediaByte() const = 0;
  virtual uint8_t SectorsPerFAT() const = 0;

  /* These values are derived from the BPB-supplied ones. */
  uint16_t FATRegionSector() const { return ReservedSectorCount(); }
  uint16_t FATRegionSize() const {
    return FATRegionCount() * SectorsPerFAT() * BytesPerSector();
  }

  uint16_t RootDirectoryRegionSector() const {
    return FATRegionSector() + (FATRegionCount() * SectorsPerFAT());
  }
  uint16_t RootDirectoryRegionSize() const {
    return (RootDirectoryEntries() * 32);
  }

  uint64_t ClusterCount() const { return SectorCount() / SectorsPerCluster(); }

  void DebugPrint() const;

 protected:
  const void* structure_;
};

/* A Standard BPB uses an 8-character OEM string. */
class Standard : public Base {
 public:
  Standard(const char* structure);
  ~Standard();

  const char* OEMString() const;
  const char* BootInstruction() const;
  uint16_t BytesPerSector() const;
  uint8_t SectorsPerCluster() const;
  uint8_t FATRegionCount() const;
  uint16_t ReservedSectorCount() const;
  uint16_t RootDirectoryEntries() const;
  uint64_t SectorCount() const;
  uint8_t MediaByte() const;
  uint8_t SectorsPerFAT() const;
};

/* An Extended BPB uses a 15-character OEM string. */
class Extended : public Base {
 public:
  Extended(const char* structure);
  ~Extended();

  const char* OEMString() const;
  const char* BootInstruction() const;
  uint16_t BytesPerSector() const;
  uint8_t SectorsPerCluster() const;
  uint8_t FATRegionCount() const;
  uint16_t ReservedSectorCount() const;
  uint16_t RootDirectoryEntries() const;
  uint64_t SectorCount() const;
  uint8_t MediaByte() const;
  uint8_t SectorsPerFAT() const;
};

}  // namespace BPB
}  // namespace Human68k
}  // namespace libFAT

#endif  //  LIBFAT_HUMAN68K_BPB_H_
