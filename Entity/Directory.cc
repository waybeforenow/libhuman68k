#include <memory>
#include <string>

#include "../lib/easyloggingpp/easylogging++.h"
#include "Entity.h"

namespace libFAT {
namespace Human68k {

Directory::Directory(std::unique_ptr<DirectoryHelper> helper)
    : helper_(std::move(helper)), is_read_(false) {
  LOG(INFO) << "Directory constructor";

  helper_->RegisterInode(this, true);
}

Directory::Directory(std::unique_ptr<DirectoryHelper> helper,
                     const char* filename, const char* extension,
                     char attributes, const char* extra_filename,
                     uint16_t mdate, uint32_t file_size)
    : helper_(std::move(helper)),
      is_read_(false),
      Entity(filename, extension, attributes, extra_filename, mdate,
             file_size) {
  LOG(INFO) << "Directory constructor 2";

  helper_->RegisterInode(this);
}

Directory::~Directory() { LOG(INFO) << "Directory destructor"; }

void Directory::CreateFile(fuse_ino_t* ino, struct stat* attr,
                           const char* name_) {
  std::string name(name_, 8), ext(name_, 8, 3), long_name;
  if (strlen(name_) > 11) {
    long_name = std::string(name_, 11, std::string::npos);
  }

  /* XXX make the cdate */
  *ino = helper_->CreateFile(name, ext, long_name, 0x20, 0);
}

void Directory::CreateDirectory(fuse_ino_t* ino, struct stat* attr,
                                const char* name_) {
  std::string name(name_, 8), ext(name_, 8, 3), long_name;
  if (strlen(name_) > 11) {
    long_name = std::string(name_, 11, std::string::npos);
  }

  /* XXX make the cdate */
  *ino = helper_->CreateFile(name, ext, long_name, 0x10, 0);
}

void Directory::RemoveFile(const char* name) { helper_->RemoveFile(name); }

void Directory::RemoveDirectory(const char* name) {
  helper_->RemoveDirectory(name);
}

void Directory::Read() {
  helper_->Read(&entries_);
  is_read_ = true;
}

bool Directory::IsRead() { return is_read_; }

std::unique_ptr<std::vector<std::pair<char*, struct stat*>>>
Directory::GetEntries(size_t size, off_t offset) {
  auto ret = std::make_unique<std::vector<std::pair<char*, struct stat*>>>();
  if (!IsRead()) {
    Read();
  }

  size_t i = 0;
  for (const auto& entry : entries_) {
    if (i < offset) {
      continue;
    } else if (i > offset + size) {
      break;
    }
    i++;

    char* fn = strdup(entry->Filename()->c_str());
    struct stat* attr = (struct stat*)malloc(sizeof(struct stat));
    entry->GetAttr(attr);
    ret->push_back(std::make_pair(fn, attr));
  }

  return std::move(ret);
}

void Directory::GetAttr(struct stat* attr) const { helper_->GetAttr(attr); }
void Directory::SetAttr(struct stat* attr, int to_set) {
  helper_->SetAttr(attr, to_set);
}

}  // namespace Human68k
}  // namespace libFAT

