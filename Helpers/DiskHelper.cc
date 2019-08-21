#include "DiskHelper.h"

#include <sys/stat.h>

#include <exception>

#include "PartitionHelper.h"

namespace libFAT {
namespace Human68k {
namespace DiskHelper {

_DiskHelper::_DiskHelper() : global_offset_(0), bytes_per_cluster_(0) {}

_DiskHelper* _DiskHelper::global_fshelper_ = nullptr;

_DiskHelper* _DiskHelper::GetDiskHelper() {
  if (!global_fshelper_) {
    global_fshelper_ = new _DiskHelper;
  }

  return global_fshelper_;
}

size_t _DiskHelper::Offset() { return global_offset_; }
void _DiskHelper::set_offset(size_t offset) { global_offset_ = offset; }

_DiskHelper* GetDiskHelper() { return _DiskHelper::GetDiskHelper(); }

size_t _DiskHelper::BytesPerCluster() { return bytes_per_cluster_; }

void _DiskHelper::RegisterBPB(BPB::Base* bpb) {
  size_t bytes_per_cluster = bpb->BytesPerSector() * bpb->SectorsPerCluster();
  if (bytes_per_cluster_ && bytes_per_cluster != bytes_per_cluster_) {
    /* Shouldn't happen */
    throw new std::exception();
  }

  bytes_per_cluster_ = bytes_per_cluster;
}

}  // namespace DiskHelper
}  // namespace Human68k
}  // namespace libFAT
