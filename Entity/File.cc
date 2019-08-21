#include <sys/stat.h>

#include <memory>

#include "../Helpers/FileHelper.h"
#include "../lib/easyloggingpp/easylogging++.h"
#include "Entity.h"

namespace libFAT {
namespace Human68k {

File::File(std::unique_ptr<FileHelper> helper, const char* filename,
           const char* extension, char attributes, const char* extra_filename,
           uint16_t mdate, uint32_t file_size)
    : helper_(std::move(helper)),
      Entity(filename, extension, attributes, extra_filename, mdate,
             file_size) {
  LOG(INFO) << "File constructor";

  helper_->RegisterInode(this);
}

size_t File::Read(void* buf, size_t size, off_t offset) {
  return this->helper_->Read((char*)buf, size, offset);
}

size_t File::Write(const void* buf, size_t size, off_t offset) {
  return this->helper_->Write((const char*)buf, size, offset);
}

void File::GetAttr(struct stat* attr) const {
  helper_->GetAttr(attr);
  attr->st_size = file_size_;
}
void File::SetAttr(struct stat* attr, int to_set) {
  helper_->SetAttr(attr, to_set);
}

}  // namespace Human68k
}  // namespace libFAT
