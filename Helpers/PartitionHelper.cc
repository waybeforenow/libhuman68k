#include "PartitionHelper.h"

#include <sys/types.h>

#include <functional>
#include <vector>

#include "../lib/easyloggingpp/easylogging++.h"

#define FUSE_USE_VERSION 31
#include "fuse_lowlevel.h"

namespace libFAT {
namespace Human68k {

fuse_ino_t PartitionHelper::GetNewInode(File* fileptr) {
  if (new_inode_fn_ == nullptr) {
    return 0;
  }

  auto new_inode = new_inode_fn_();
  entity_by_inode_[new_inode] = fileptr;
  entity_by_name_[*(fileptr->Filename())] = fileptr;

  return new_inode;
}

fuse_ino_t PartitionHelper::GetNewInode(Directory* dirptr, bool is_root_dir) {
  if (new_inode_fn_ == nullptr) {
    return 0;
  }

  auto new_inode = new_inode_fn_();
  dir_by_inode_[new_inode] = dirptr;
  entity_by_inode_[new_inode] = dirptr;
  if (!is_root_dir) {
    entity_by_name_[*(dirptr->Filename())] = dirptr;
  }

  return new_inode;
}

PartitionHelper::PartitionHelper(std::function<fuse_ino_t()> new_inode_fn)
    : new_inode_fn_(new_inode_fn), DiskHelper::InstantiatedWithOffset() {
  LOG(INFO) << "PartitionHelper constructor";
}

PartitionHelper::~PartitionHelper() {
  LOG(INFO) << "PartitionHelper destructor";
}

Entity* PartitionHelper::GetEntity(fuse_ino_t ino) {
  auto entry = entity_by_inode_.find(ino);
  if (entry == entity_by_inode_.end()) {
    LOG(INFO) << "No entity found; returning nullptr";
    return nullptr;
  }
  LOG(INFO) << "Entity found; returning it";
  return entry->second;
}

Entity* PartitionHelper::GetEntity(const char* filename) {
  auto entry = entity_by_name_.find(filename);
  if (entry == entity_by_name_.end()) {
    return nullptr;
  }
  return entry->second;
}

Directory* PartitionHelper::GetDirectory(fuse_ino_t ino) {
  auto entry = dir_by_inode_.find(ino);
  if (entry == dir_by_inode_.end()) {
    return nullptr;
  }
  return entry->second;
}

}  // namespace Human68k
}  // namespace libFAT
