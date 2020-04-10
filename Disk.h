#ifndef LIBFAT_HUMAN68K_DISK_H_
#define LIBFAT_HUMAN68K_DISK_H_

#include <stdint.h>
#include <stdio.h>
#include <sys/statvfs.h>

#include <mutex>
#include <vector>

#include "FUSEShims.h"
#include "Partition.h"

namespace libFAT {
namespace Human68k {

/* @brief A single disk
 *
 * A Disk represents a single disk. It encapsulates one or more partitions. */
class Disk {
 public:
  Disk(const char* block_device);
  ~Disk();

  Partition* GetPartitionByIndex(ssize_t index) const;
  Partition* GetPartitionByInode(fuse_ino_t inode) const;

  int Flush();

  constexpr bool HasPartitionTable() const { return has_partition_table_; }
  const char* BlockDeviceName() const { return block_device_name_; }

  enum Type {
    FLOPPY,
    FLOPPY_DIM,
    SCSI,
  };
  Type FSType() const { return fs_type_; }

 private:
  FILE* block_fp_;
  const char* block_device_name_;

  bool has_partition_table_;
  uint8_t flags_;
  uint64_t unalloc_sector_start_;
  uint64_t last_sector_;

  std::vector<Partition*> partitions_;

  void init_partitions_();
  bool plausible_partition_table_at(size_t offset) const;

  std::vector<size_t> inode_to_partition_;
  fuse_ino_t GetNewInode_(size_t partition_num);

  Type fs_type_;
};

}  // namespace Human68k
}  // namespace libFAT

#endif  //  LIBFAT_HUMAN68K_DISK_H_
