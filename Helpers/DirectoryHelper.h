#ifndef LIBFAT_HUMAN68K_HELPERS_DIRECTORYHELPER_H_
#define LIBFAT_HUMAN68K_HELPERS_DIRECTORYHELPER_H_

#include "../FAT.h"

#define FUSE_USE_VERSION 31
#include "fuse_lowlevel.h"

namespace libFAT {
namespace Human68k {

class Entity;
class Directory;
class PartitionHelper;

class DirectoryHelper {
 public:
  DirectoryHelper(FILE* file, FAT* fat, PartitionHelper* partition_helper,
                  size_t stat_offset, size_t starting_cluster);
  ~DirectoryHelper();
  void RegisterInode(Directory* dirptr, bool is_root_dir = false);

  void Read(std::vector<Entity*>* entities);

  fuse_ino_t CreateFile(const std::string& name, const std::string& ext,
                        const std::string& long_name, char attributes,
                        uint16_t cdate) const;
  void RemoveFile(const char* name) const;
  void RemoveDirectory(const char* name) const;

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

#endif  // LIBFAT_HUMAN68K_HELPERS_DIRECTORYHELPER_H_
