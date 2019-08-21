#include "DirectoryHelper.h"

#include "../Entity/Entity.h"
#include "../lib/easyloggingpp/easylogging++.h"
#include "DiskHelper.h"
#include "FileHelper.h"
#include "PartitionHelper.h"

namespace libFAT {
namespace Human68k {

DirectoryHelper::DirectoryHelper(FILE* file, FAT* fat,
                                 PartitionHelper* partition_helper,
                                 size_t stat_offset, size_t starting_cluster)
    : file_(file),
      fat_(fat),
      partition_helper_(partition_helper),
      stat_offset_(stat_offset),
      starting_cluster_(starting_cluster) {
  LOG(INFO) << "DirectoryHelper constructor";
}

DirectoryHelper::~DirectoryHelper() {
  LOG(INFO) << "DirectoryHelper destructor";
}

void DirectoryHelper::RegisterInode(Directory* dirptr, bool is_root_dir) {
  inode_ = partition_helper_->GetNewInode(dirptr, is_root_dir);
}

void DirectoryHelper::Read(std::vector<Entity*>* entities) {
  fseek(file_, partition_helper_->GetOffset(), SEEK_SET);
  LOG(INFO) << "Partition offset: " << partition_helper_->GetOffset();
  fseek(file_,
        starting_cluster_ * DiskHelper::GetDiskHelper()->BytesPerCluster(),
        SEEK_CUR);
  LOG(INFO) << "Starting cluster offset: "
            << starting_cluster_ *
                   DiskHelper::GetDiskHelper()->BytesPerCluster();

  /* Populate the dir */
  size_t current_cluster = starting_cluster_;
  for (size_t i = 0; true; i++) {
    LOG(INFO) << "Offset: " << ftell(file_);
    if (i == DiskHelper::GetDiskHelper()->BytesPerCluster() / 32) {
      /* We've reached the end of the cluster; get the next one from the FAT */
      current_cluster = fat_->GetEntry(current_cluster);
      fseek(file_, partition_helper_->GetOffset(), SEEK_SET);
      fseek(file_,
            current_cluster * DiskHelper::GetDiskHelper()->BytesPerCluster(),
            SEEK_CUR);
      i = 0;
    }

    char dir_ent[0x20];
    fread(&dir_ent, 0x20, 1, file_);
    if (!dir_ent[0]) {
      break;
    }

    uint16_t starting_cluster;
    {
      uint8_t b1 = dir_ent[0x1b], b2 = dir_ent[0x1a];
      starting_cluster = (b1 << 8) | b2;
    }
    LOG(INFO) << "Starting cluster: " << starting_cluster;

    char filename[8], extension[3], attributes = dir_ent[0x0b],
                                    extra_filename[4];
    strncpy(filename, dir_ent, 8);
    LOG(INFO) << "Filename: " << filename;
    strncpy(extension, dir_ent + 8, 3);
    LOG(INFO) << "Extension: " << extension;
    strncpy(extra_filename, dir_ent + 0x0c, 4);
    LOG(INFO) << "Extra filename: " << extra_filename;
    /* XXX is this mdate ok? */
    uint16_t mdate;
    {
      uint8_t b1 = dir_ent[0x18], b2 = dir_ent[0x19];
      mdate = (b1 << 8) | b2;
    }
    uint32_t file_size;
    {
      uint8_t b1 = dir_ent[0x1f], b2 = dir_ent[0x1e], b3 = dir_ent[0x1d],
              b4 = dir_ent[0x1c];
      file_size = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
    }
    LOG(INFO) << "File size: " << file_size;

    if (dir_ent[0x0b] & 0x08) {
      /* the volume name bit is set in the attribute byte */
      /* currently we just ignore it */
    } else if (dir_ent[0x0b] & 0x10) {
      /* the directory bit is set in the attribute byte */
      auto dir_helper = std::make_unique<DirectoryHelper>(
          file_, fat_, partition_helper_,
          ftell(file_) - partition_helper_->GetOffset() - 0x20,
          starting_cluster + 9);
      entities->push_back(new Directory(std::move(dir_helper), filename,
                                        extension, attributes, extra_filename,
                                        mdate, file_size));
    } else {
      auto file_helper = std::make_unique<FileHelper>(
          file_, fat_, partition_helper_,
          ftell(file_) - partition_helper_->GetOffset() - 0x20,
          starting_cluster);
      entities->push_back(new File(std::move(file_helper), filename, extension,
                                   attributes, extra_filename, mdate,
                                   file_size));
    }
  }
}

fuse_ino_t DirectoryHelper::CreateFile(const std::string& name,
                                       const std::string& ext,
                                       const std::string& long_name,
                                       char attributes, uint16_t cdate) const {
  LOG(INFO) << "DirectoryHelper::CreateFile";
  /* XXX */
  return 0;
}

void DirectoryHelper::RemoveFile(const char* name) const { /* XXX */
}

void DirectoryHelper::RemoveDirectory(const char* name) const { /* XXX */
}

void DirectoryHelper::GetAttr(struct stat* attr) const {
  attr->st_dev = 0;
  attr->st_ino = inode_;
  attr->st_mode = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
  attr->st_nlink = 1;
  attr->st_uid = 0;
  attr->st_gid = 0;

  /* XXX */
  attr->st_size = 0;
  attr->st_atime = 0;
  attr->st_mtime = 0;
  attr->st_ctime = 0;
  attr->st_blksize = 1024;
  attr->st_blocks = 1;
}
void DirectoryHelper::SetAttr(struct stat* attr, int to_set) { /* XXX */
}

}  // namespace Human68k
}  // namespace libFAT
