#ifndef LIBFAT_HUMAN68K_HELPERS_FILEHELPER_H_
#define LIBFAT_HUMAN68K_HELPERS_FILEHELPER_H_

#include <memory>
#include <string>
#include <utility>

#include "../FAT.h"

#define FUSE_USE_VERSION 31
#include "fuse_lowlevel.h"

namespace libFAT {
namespace Human68k {

class File;
class PartitionHelper;

class FileHelper {
 public:
  FileHelper(FILE* file, FAT* fat, PartitionHelper* partition_helper,
             size_t stat_offset, size_t starting_cluster);
  ~FileHelper();
  void RegisterInode(File* fileptr);

  size_t Read(char* buf, size_t size, off_t offset);
  size_t Write(const char* buf, size_t size, off_t offset);

  void GetAttr(struct stat* attr) const;
  void SetAttr(struct stat* attr, int to_set);

 private:
  FILE* file_;
  FAT* fat_;
  PartitionHelper* partition_helper_;
  const size_t stat_offset_;
  const size_t starting_cluster_;
  fuse_ino_t inode_;
};

}  // namespace Human68k
}  // namespace libFAT

#endif  // LIBFAT_HUMAN68K_HELPERS_FILEHELPER_H_
