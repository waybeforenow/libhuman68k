#include "Partition.h"

#include <stdio.h>

#include <memory>

#include "FUSEShims.h"
#include "Helpers/DiskHelper.h"
#include "Helpers/PartitionHelper.h"
#include "lib/easyloggingpp/easylogging++.h"

namespace libFAT {
namespace Human68k {

Partition::Partition(FILE* block_fp, PartitionHelper* helper)
    : block_fp_(block_fp), helper_(helper) {
  LOG(INFO) << "Partition constructor";

  /* BIOS Parameter Block is a max of 0x22 bytes */
  char bpb_buf[0x22];
  fseek(block_fp_, helper_->GetOffset(), SEEK_SET);
  fread(&bpb_buf, 0x22, 1, block_fp_);

  /* Infer the BPB type from the media byte */
  if ((uint8_t)(bpb_buf[0x15]) > 0xe0) {
    bpb_ = new BPB::Standard(bpb_buf);
  } else {
    bpb_ = new BPB::Extended(bpb_buf);
  }
  bpb_->DebugPrint();
  DiskHelper::GetDiskHelper()->RegisterBPB(bpb_);
  helper_->SetDataOffset(
      helper_->GetOffset() +
      (bpb_->RootDirectoryRegionSector() * bpb_->BytesPerSector()));
  LOG(INFO) << "Data offset is " << helper_->GetDataOffset();

  /* Initialize the FAT */
  size_t fat_size = bpb_->FATRegionSize();
  fseek(block_fp_, helper_->GetOffset(), SEEK_SET);
  // fseek(block_fp_, bpb_->FATRegionSector() * bpb_->BytesPerSector(),
  // SEEK_CUR);
  fseek(block_fp_, bpb_->BytesPerSector(), SEEK_CUR);
  char* fat_buf = (char*)calloc(fat_size, sizeof(char));
  fread(fat_buf, 1, fat_size, block_fp_);
  fat_ = new FAT(fat_buf, fat_size);
  free(fat_buf);

  fat_->DebugPrint();

  /* Populate the root dir */
  auto root_dir_helper = std::make_unique<DirectoryHelper>(
      block_fp_, fat_, helper_, 0, bpb_->RootDirectoryRegionSector());
  root_directory_ = new Directory(std::move(root_dir_helper));
  root_directory_->Read();
}

Partition::~Partition() {
  LOG(INFO) << "Partition destructor";

  // delete fat_;
  delete bpb_;
  delete root_directory_;
  delete helper_;
}

Entity* Partition::GetEntity(fuse_ino_t ino) { return helper_->GetEntity(ino); }

Entity* Partition::GetEntity(const char* filename) {
  return helper_->GetEntity(filename);
}

Directory* Partition::GetDirectory(fuse_ino_t ino) {
  return helper_->GetDirectory(ino);
}

/* XXX implement this */
void Partition::StatFs(struct statvfs* stbuf) const {
  /* pointers @ http://man7.org/linux/man-pages/man3/statvfs.3.html */
}

}  // namespace Human68k
}  // namespace libFAT
