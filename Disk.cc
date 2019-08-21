#include "Disk.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>

#include <functional>
#include <string>

#include "Helpers/DiskHelper.h"
#include "Helpers/PartitionHelper.h"
#include "Partition.h"
#include "lib/easyloggingpp/easylogging++.h"

namespace libFAT {
namespace Human68k {

namespace {

/* case-insensitive string matching */
bool iequals(const std::string& a, const std::string& b) {
  return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                    [](char a, char b) { return tolower(a) == tolower(b); });
}

}  // namespace

Disk::Disk(const char* block_device) {
  LOG(INFO) << "Disk constructor";
  block_device_name_ = strdup(block_device);
  block_fp_ = fopen(block_device, "rb");
  if (block_fp_ == nullptr) {
    exit(0);
  }

  std::string bd_ext(block_device, strlen(block_device) - 4, std::string::npos);
  if (iequals(bd_ext, ".dim")) {
    this->fs_type_ = FLOPPY_DIM;
  } else if (iequals(bd_ext, ".xdf")) {
    this->fs_type_ = FLOPPY;
  } else {
    this->fs_type_ = SCSI;
  }

  size_t npartitions = 0;
  if (this->fs_type_ == SCSI) {
    /* image contains multiple partitions - defer to init_partitions_ to
     * initialize them.*/
    this->init_partitions_();
  } else {
    /* image contains a single initializable partition */
    DiskHelper::JumpOffset jump((this->fs_type_ == FLOPPY_DIM) ? 0x100 : 0);

    auto helper =
        new PartitionHelper(std::bind(&Disk::GetNewInode_, this, npartitions));
    partitions_.push_back(new Partition(block_fp_, helper));
  }
}

Disk::~Disk() {
  LOG(INFO) << "Disk destructor";

  fclose(block_fp_);
  for (auto& partition : partitions_) {
    delete partition;
  }
}

void Disk::init_partitions_() {
  // XXX
}

Partition* Disk::GetPartitionByIndex(ssize_t index) const {
  return partitions_.at(index);
}

Partition* Disk::GetPartitionByInode(fuse_ino_t inode) const {
  if (HasPartitionTable()) {
    return partitions_.at(inode_to_partition_.at(inode - FUSE_ROOT_ID));
  }

  return partitions_.at(0);
}

int Disk::Flush() {
  if (fflush(block_fp_) != 0) {
    return -errno;
  }
  return 0;
}

bool Disk::plausible_partition_table_at(size_t offset) const {
  char check[4];
  const char expect[] = "X68k";

  fread(&check, 4, 1, block_fp_);
  return (!strncmp(expect, check, 4));
}

fuse_ino_t Disk::GetNewInode_(size_t partition_num) {
  size_t elems = inode_to_partition_.size();
  LOG(INFO) << "Disk is giving out a new inode: " << elems + FUSE_ROOT_ID;

  inode_to_partition_.push_back(partition_num);
  return elems + FUSE_ROOT_ID;
}

}  // namespace Human68k
}  // namespace libFAT

