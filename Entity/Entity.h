#ifndef LIBFAT_HUMAN68K_ENTITY_ENTITY_H_
#define LIBFAT_HUMAN68K_ENTITY_ENTITY_H_

#include <stdint.h>
#include <sys/stat.h>

#include <functional>
#include <memory>
#include <vector>

#include "../Helpers/DirectoryHelper.h"
#include "../Helpers/FileHelper.h"

#define FUSE_USE_VERSION 31
#include "fuse_lowlevel.h"

namespace libFAT {
namespace Human68k {

class Entity {
 public:
  Entity() = default;
  virtual ~Entity() = default;

  std::unique_ptr<std::string> Filename();

  const char* RawFilename() { return filename_; }
  const char* Extension() { return extension_; }
  bool IsReadOnly() const { return attributes_ & 0x01; }
  bool IsHidden() const { return attributes_ & 0x02; }
  bool IsSystem() const { return attributes_ & 0x04; }
  bool IsVolumeLabel() const { return attributes_ & 0x08; }
  bool IsArchive() const { return attributes_ & 0x20; }
  bool IsDevice() const { return attributes_ & 0x40; }
  uint32_t FileSize() const { return file_size_; }
  virtual void GetAttr(struct stat* attr) const = 0;
  virtual void SetAttr(struct stat* attr, int to_set) = 0;

 protected:
  Entity(const char* filename, const char* extension, char attributes,
         const char* extra_filename, uint16_t mdate, uint32_t file_size)
      : filename_(strndup(filename, 8)),
        extension_(strndup(extension, 3)),
        /* XXX how long should this be?        vv */
        extra_filename_(strndup(extra_filename, 4)),
        attributes_(attributes),
        create_date_(mdate),
        file_size_(file_size) {}
  char* filename_;
  char* extension_;
  char* extra_filename_;
  char attributes_;
  uint32_t create_time_;
  uint16_t create_date_;
  uint16_t access_date_;
  uint16_t modified_time_;
  uint16_t modified_date_;
  uint32_t file_size_;
};

class File : public Entity {
 public:
  File(std::unique_ptr<FileHelper> helper, const char* filename,
       const char* extension, char attributes, const char* extra_filename,
       uint16_t mdate, uint32_t file_size);
  ~File() = default;

  size_t Read(void* buf, size_t size, off_t offset);
  size_t Write(const void* buf, size_t size, off_t offset);

  void GetAttr(struct stat* attr) const;
  void SetAttr(struct stat* attr, int to_set);

 private:
  std::unique_ptr<FileHelper> helper_;
};

class Directory : public Entity {
 public:
  Directory(std::unique_ptr<DirectoryHelper> helper);
  Directory(std::unique_ptr<DirectoryHelper> helper, const char* filename,
            const char* extension, char attributes, const char* extra_filename,
            uint16_t mdate, uint32_t file_size);
  ~Directory();

  void Read();
  bool IsRead();

  std::unique_ptr<std::vector<std::pair<char*, struct stat*>>> GetEntries(
      size_t size, off_t offset);
  void CreateFile(fuse_ino_t* ino, struct stat* attr, const char* name);
  void CreateDirectory(fuse_ino_t* ino, struct stat* attr, const char* name);
  void RemoveFile(const char* name);
  void RemoveDirectory(const char* name);

  void GetAttr(struct stat* attr) const;
  void SetAttr(struct stat* attr, int to_set);

 private:
  std::unique_ptr<DirectoryHelper> helper_;
  std::vector<Entity*> entries_;

  bool is_read_;
};

}  // namespace Human68k
}  // namespace libFAT

#endif  // LIBFAT_HUMAN68K_ENTITY_H_
