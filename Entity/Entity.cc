#include "Entity.h"

#include <sys/stat.h>

#include <memory>

namespace libFAT {
namespace Human68k {

std::unique_ptr<std::string> Entity::Filename() {
  const char* space_pos = strchr(filename_, ' ');
  size_t fn_len = (!space_pos) ? 8 : space_pos - filename_;
  auto ret = std::make_unique<std::string>(filename_, fn_len);
  if (ret->at(0) == 0x05) {
    ret->at(0) = 0xe5;
  }
  if (extra_filename_[0] != 0x00) {
    space_pos = strchr(extra_filename_, 0);
    fn_len = space_pos - extra_filename_;
    ret->append(extra_filename_, fn_len);
  }
  if (extension_[0] != 0x20) {
    space_pos = strchr(extension_, ' ');
    fn_len = (!space_pos) ? 3 : space_pos - extension_;
    ret->push_back('.');
    ret->append(extension_, fn_len);
  }
  return std::move(ret);
}

}  // namespace Human68k
}  // namespace libFAT
