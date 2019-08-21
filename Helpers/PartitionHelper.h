#ifndef LIBFAT_HUMAN68K_HELPERS_PARTITIONHELPER_H_
#define LIBFAT_HUMAN68K_HELPERS_PARTITIONHELPER_H_

#include <sys/types.h>

#include <functional>
#include <vector>

#include "../Entity/Entity.h"
#include "../FUSEShims.h"
#include "DiskHelper.h"

namespace libFAT {
namespace Human68k {

class PartitionHelper : public DiskHelper::InstantiatedWithOffset {
 public:
  fuse_ino_t GetNewInode(File* fileptr);
  fuse_ino_t GetNewInode(Directory* dirptr, bool is_root_dir);
  ~PartitionHelper();

  Entity* GetEntity(fuse_ino_t ino);
  Entity* GetEntity(const char* filename);

  Directory* GetDirectory(fuse_ino_t ino);

  size_t GetDataOffset() const { return data_offset_; }

 private:
  friend class Disk;
  friend class Partition;

  PartitionHelper(std::function<fuse_ino_t()> new_inode_fn);

  off_t partition_number_;
  std::function<fuse_ino_t()> new_inode_fn_;

  std::unordered_map<std::string, Entity*> entity_by_name_;
  std::unordered_map<fuse_ino_t, Entity*> entity_by_inode_;
  std::unordered_map<fuse_ino_t, Directory*> dir_by_inode_;

  size_t data_offset_;
  void SetDataOffset(size_t offset) { data_offset_ = offset; }
};

}  // namespace Human68k
}  // namespace libFAT

#endif  // LIBFAT_HUMAN68K_HELPERS_PARTITIONHELPER_H_
