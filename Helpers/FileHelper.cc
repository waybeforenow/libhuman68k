#include "FileHelper.h"

#include <sys/stat.h>

#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "../FAT.h"
#include "../lib/easyloggingpp/easylogging++.h"
#include "PartitionHelper.h"

namespace libFAT {
namespace Human68k {

FileHelper::FileHelper(FILE* file, FAT* fat, PartitionHelper* partition_helper,
                       size_t stat_offset, size_t starting_cluster)
    : file_(file),
      fat_(fat),
      partition_helper_(partition_helper),
      stat_offset_(stat_offset),
      starting_cluster_(starting_cluster) {
  LOG(INFO) << "FileHelper constructor";
}

FileHelper::~FileHelper() { LOG(INFO) << "FileHelper destructor"; }

void FileHelper::RegisterInode(File* fileptr) {
  inode_ = partition_helper_->GetNewInode(fileptr);
}

size_t FileHelper::Read(char* buf, size_t size, off_t offset) {
  LOG(INFO) << "Reading " << size << " bytes at offset " << offset;
  size_t cluster = starting_cluster_;
  size_t ret = 0;

  while (offset > DiskHelper::GetDiskHelper()->BytesPerCluster()) {
    cluster = fat_->GetEntry(cluster);
    if (cluster == 0x000) {
      LOG(WARNING) << "Reached unexpected OOB on FAT";
      return ret;
    }
    offset -= DiskHelper::GetDiskHelper()->BytesPerCluster();
  }

  // seek to the starting cluster
  fseek(file_, partition_helper_->GetDataOffset(), SEEK_SET);
  fseek(
      file_,
      ((cluster + 4) * DiskHelper::GetDiskHelper()->BytesPerCluster()) + offset,
      SEEK_CUR);
  LOG(INFO) << "Seeking to offset " << std::hex << ftell(file_);
  while (offset + size > DiskHelper::GetDiskHelper()->BytesPerCluster()) {
    if (feof(file_)) {
      LOG(WARNING) << "Reached unexpected EOF";
      return ret;
    }
    size_t tret = fread(
        buf, 1, DiskHelper::GetDiskHelper()->BytesPerCluster() - offset, file_);
    ret += tret;
    size -= tret;
    buf += tret;
    offset = 0;
    LOG(INFO) << "FAT entry at cluster: " << std::hex
              << fat_->GetEntry(cluster);
    cluster = fat_->GetEntry(cluster);
    if (cluster == 0x000) {
      LOG(WARNING) << "Reached unexpected OOB on FAT";
      return ret;
    }
    fseek(file_, partition_helper_->GetDataOffset(), SEEK_SET);
    fseek(file_,
          ((cluster + 4) * DiskHelper::GetDiskHelper()->BytesPerCluster()),
          SEEK_CUR);
    LOG(INFO) << "Seeking to offset " << std::hex << ftell(file_);
  }

  // read the remainder of the data
  if (feof(file_)) {
    LOG(WARNING) << "Reached unexpected EOF";
    return ret;
  }
  ret += fread(buf, 1, size, file_);
  return ret;
}

size_t FileHelper::Write(const char* buf, size_t size, off_t offset) {
  LOG(INFO) << "Writing " << size << " bytes at offset " << offset;
  size_t cluster = starting_cluster_;
  size_t ret = 0;

  while (offset > DiskHelper::GetDiskHelper()->BytesPerCluster()) {
    cluster = fat_->GetEntry(cluster);
    if (cluster == 0x000) {
      LOG(WARNING) << "Reached unexpected OOB on FAT";
      return ret;
    }
    offset -= DiskHelper::GetDiskHelper()->BytesPerCluster();
  }

  // seek to the starting cluster
  fseek(file_, partition_helper_->GetDataOffset(), SEEK_SET);
  fseek(
      file_,
      ((cluster + 4) * DiskHelper::GetDiskHelper()->BytesPerCluster()) + offset,
      SEEK_CUR);
  LOG(INFO) << "Seeking to offset " << std::hex << ftell(file_);
  while (offset + size > DiskHelper::GetDiskHelper()->BytesPerCluster()) {
    if (feof(file_)) {
      LOG(WARNING) << "Reached unexpected EOF";
      return ret;
    }
    size_t tret = fwrite(
        buf, DiskHelper::GetDiskHelper()->BytesPerCluster() - offset, 1, file_);
    ret += tret;
    size -= tret;
    buf += tret;
    offset = 0;
    LOG(INFO) << "FAT entry at cluster: " << std::hex
              << fat_->GetEntry(cluster);
    cluster = fat_->GetEntry(cluster);
    if (cluster == 0x000) {
      LOG(WARNING) << "Reached unexpected OOB on FAT";
      return ret;
    }
    fseek(file_, partition_helper_->GetDataOffset(), SEEK_SET);
    fseek(file_,
          ((cluster + 4) * DiskHelper::GetDiskHelper()->BytesPerCluster()),
          SEEK_CUR);
  }

  // write the remainder of the data
  if (feof(file_)) {
    LOG(WARNING) << "Reached unexpected EOF";
    return ret;
  }
  ret += fwrite(buf, size, 1, file_);
  return ret;
}

void FileHelper::GetAttr(struct stat* attr) const {
  attr->st_ino = inode_;
  attr->st_mode = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;
  attr->st_nlink = 1;
  attr->st_blksize = 1024;
  attr->st_blocks = 1;
}
void FileHelper::SetAttr(struct stat* attr, int to_set) { /* XXX */
}

}  // namespace Human68k
}  // namespace libFAT
