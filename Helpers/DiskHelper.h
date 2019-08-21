#ifndef LIBFAT_HUMAN68K_HELPERS_DISKHELPER_H_
#define LIBFAT_HUMAN68K_HELPERS_DISKHELPER_H_

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../BPB.h"
#include "../lib/easyloggingpp/easylogging++.h"

namespace libFAT {
namespace Human68k {
namespace DiskHelper {

class _DiskHelper {
 public:
  static _DiskHelper* GetDiskHelper();
  size_t Offset();

  size_t BytesPerCluster();
  void RegisterBPB(BPB::Base* bpb);

 private:
  friend class Disk;
  friend class JumpOffset;
  friend class AbsoluteOffset;

  _DiskHelper();

  size_t global_offset_;
  void set_offset(size_t offset);

  size_t bytes_per_cluster_;

  static _DiskHelper* global_fshelper_;
};

_DiskHelper* GetDiskHelper();

class JumpOffset {
 public:
  JumpOffset(int offset) : offset_(offset) {
    LOG(INFO) << "JumpOffset constructor with offset " << offset;
    GetDiskHelper()->set_offset(GetDiskHelper()->Offset() + offset);
  }

  ~JumpOffset() {
    LOG(INFO) << "JumpOffset destructor";
    GetDiskHelper()->set_offset(GetDiskHelper()->Offset() - offset_);
  }

 private:
  int offset_;
};

class AbsoluteOffset {
 public:
  AbsoluteOffset(int offset) : prev_offset_(GetDiskHelper()->Offset()) {
    GetDiskHelper()->set_offset(offset);
  }

  ~AbsoluteOffset() { GetDiskHelper()->set_offset(prev_offset_); }

 private:
  size_t prev_offset_;
};

class InstantiatedWithOffset {
 public:
  size_t GetOffset() const { return offset_; }

 protected:
  InstantiatedWithOffset() : offset_(DiskHelper::GetDiskHelper()->Offset()) {
    LOG(INFO) << "InstantiatedWithOffset constructor, offset " << offset_;
  }

 private:
  size_t offset_;
};

}  // namespace DiskHelper
}  // namespace Human68k
}  // namespace libFAT

#endif  // LIBFAT_HUMAN68K_HELPERS_DISKHELPER_H_
