#ifndef LIBFAT_HUMAN68K_PARTITION_H_
#define LIBFAT_HUMAN68K_PARTITION_H_

#include <stdio.h>
#include <sys/statvfs.h>

#include <queue>
#include <unordered_map>
#include <vector>

#include "BPB.h"
#include "Entity/Entity.h"
#include "FAT.h"
#include "FUSEShims.h"
#include "Helpers/PartitionHelper.h"

namespace libFAT {
namespace Human68k {

/* @brief A single logical partition
 *
 * A Partition represents a single logical partition on a disk. It encapsulates
 * its own FAT, block device file, BPB, PartitionHelper and root Directory.
 */
class Partition {
 public:
  Partition(FILE* block_fp, PartitionHelper* helper);
  ~Partition();

  Entity* GetEntity(fuse_ino_t ino);
  Entity* GetEntity(const char* filename);

  Directory* GetDirectory(fuse_ino_t ino);

  void StatFs(struct statvfs* stbuf) const;

 private:
  FILE* block_fp_;

  FAT* fat_;
  BPB::Base* bpb_;
  Directory* root_directory_;

  uint8_t flags_;
  int64_t start_sector_;
  uint64_t sector_count_;
  PartitionHelper* helper_;
};

}  // namespace Human68k
}  // namespace libFAT

#endif  // LIBFAT_HUMAN68K_PARTITION_H_
