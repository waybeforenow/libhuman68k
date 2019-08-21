#include "FUSEOps.h"

#include <errno.h>
#include <sys/stat.h>

#include <memory>

#define FUSE_USE_VERSION 31
#include "../BPB.h"
#include "../Disk.h"
#include "../Entity/Entity.h"
#include "../FAT.h"
#include "../lib/easyloggingpp/easylogging++.h"
#include "fuse_lowlevel.h"

namespace libFAT {
namespace Human68k {

void FUSEOps::Init(void *userdata, struct fuse_conn_info *conn) {
  LOG(INFO) << "FUSEOps::Init";
  auto fs_info = (UserData *)userdata;
  auto disk = std::make_unique<Disk>(fs_info->file_name);

  fs_info->disk = std::move(disk);
}

void FUSEOps::Destroy(void *userdata) {
  LOG(INFO) << "FUSEOps::Destroy";
  auto fs_info = (UserData *)userdata;
  fs_info->disk.release();
}

void FUSEOps::Lookup(fuse_req_t req, fuse_ino_t parent, const char *name) {
  LOG(INFO) << "FUSEOps::Lookup " << parent << " " << name;
  auto fs_info = (UserData *)fuse_req_userdata(req);
  fuse_ino_t ino;

  auto file = fs_info->disk->GetPartitionByInode(parent)->GetEntity(name);
  if (file == nullptr) {
    fuse_reply_err(req, ENOENT);
    return;
  }

  struct stat attr;
  file->GetAttr(&attr);

  fuse_entry_param entry_reply = {attr.st_ino, attr.st_ino, attr, 3000., 3000.};
  fuse_reply_entry(req, &entry_reply);
}

void FUSEOps::Forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup) {
  LOG(INFO) << "FUSEOps::Forget";
  fuse_reply_none(req);
}

void FUSEOps::GetAttr(fuse_req_t req, fuse_ino_t ino,
                      struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::GetAttr " << ino;
  auto fs_info = (UserData *)fuse_req_userdata(req);
  struct stat attr;
  auto ent = fs_info->disk->GetPartitionByInode(ino)->GetEntity(ino);
  if (ent == nullptr) {
    fuse_reply_err(req, ENOENT);
    return;
  }

  ent->GetAttr(&attr);
  LOG(INFO) << "Inode: " << attr.st_ino;
  LOG(INFO) << "Mode:  " << attr.st_mode;
  LOG(INFO) << "Size:  " << attr.st_size;
  fuse_reply_attr(req, &attr, 3000.);
}

void FUSEOps::SetAttr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
                      int to_set, struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::SetAttr";
  auto fs_info = (UserData *)fuse_req_userdata(req);
  fs_info->disk->GetPartitionByInode(ino)->GetEntity(ino)->SetAttr(attr,
                                                                   to_set);
  fuse_reply_attr(req, attr, 3000.);
}

void FUSEOps::MkNod(fuse_req_t req, fuse_ino_t parent, const char *name,
                    mode_t mode, dev_t rdev) {
  LOG(INFO) << "FUSEOps::MkNod";
  auto fs_info = (UserData *)fuse_req_userdata(req);
  auto directory =
      fs_info->disk->GetPartitionByInode(parent)->GetDirectory(parent);

  fuse_ino_t ino;
  struct stat attr;

  directory->CreateFile(&ino, &attr, name);
  fuse_entry_param entry_reply = {ino, ino, attr, 3000., 3000.};
  fuse_reply_entry(req, &entry_reply);
}

void FUSEOps::MkDir(fuse_req_t req, fuse_ino_t parent, const char *name,
                    mode_t mode) {
  LOG(INFO) << "FUSEOps::MkDir";
  auto fs_info = (UserData *)fuse_req_userdata(req);
  auto directory =
      fs_info->disk->GetPartitionByInode(parent)->GetDirectory(parent);

  fuse_ino_t ino;
  struct stat attr;
  directory->CreateDirectory(&ino, &attr, name);

  fuse_entry_param entry_reply = {ino, ino, attr, 3000., 3000.};
  fuse_reply_entry(req, &entry_reply);
}

void FUSEOps::Unlink(fuse_req_t req, fuse_ino_t parent, const char *name) {
  LOG(INFO) << "FUSEOps::Unlink";
  auto fs_info = (UserData *)fuse_req_userdata(req);
  auto directory =
      fs_info->disk->GetPartitionByInode(parent)->GetDirectory(parent);

  directory->RemoveFile(name);
}

void FUSEOps::RmDir(fuse_req_t req, fuse_ino_t parent, const char *name) {
  LOG(INFO) << "FUSEOps::RmDir";
  auto fs_info = (UserData *)fuse_req_userdata(req);
  auto directory =
      fs_info->disk->GetPartitionByInode(parent)->GetDirectory(parent);

  directory->RemoveDirectory(name);
}

void FUSEOps::Read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                   struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::Read";
  auto fs_info = (UserData *)fuse_req_userdata(req);
  auto file = dynamic_cast<File *>(
      fs_info->disk->GetPartitionByInode(ino)->GetEntity(ino));

  if (file == nullptr) {
    fuse_reply_err(req, ENOENT);
    return;
  }
  char *buf = (char *)malloc(size);
  auto rsize = file->Read(buf, size, off);

  fuse_reply_buf(req, buf, rsize);
  free(buf);
}

void FUSEOps::Write(fuse_req_t req, fuse_ino_t ino, const char *buf,
                    size_t size, off_t off, struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::Write";
  auto fs_info = (UserData *)fuse_req_userdata(req);
  auto file = dynamic_cast<File *>(
      fs_info->disk->GetPartitionByInode(ino)->GetEntity(ino));

  if (fi->flags & (O_WRONLY | O_RDWR) && file->IsReadOnly()) {
    fuse_reply_err(req, EACCES);
    return;
  }

  auto rsize = file->Write(buf, size, off);
  fuse_reply_write(req, rsize);
}

void FUSEOps::Flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::Flush";
  auto fs_info = (UserData *)fuse_req_userdata(req);
  int ret;
  if ((ret = fs_info->disk->Flush())) {
    fuse_reply_err(req, ret);
  }
}

void FUSEOps::ReadDir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                      struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::ReadDir " << size << " " << off;
  auto fs_info = (UserData *)fuse_req_userdata(req);
  auto partition = fs_info->disk->GetPartitionByInode(ino);
  auto dir = partition->GetDirectory(ino);

  const size_t bufmax = 1024 * 16;
  char buf[bufmax];
  size_t bufsize = 0, diroffset = 0;

  auto entries = dir->GetEntries(size, off);

  for (const auto &entry : *entries) {
    LOG(INFO) << "Adding entry: " << entry.first << "(" << entry.second->st_ino
              << ")";
    size_t entrysize =
        fuse_add_direntry(req, buf + bufsize, bufmax - bufsize, entry.first,
                          entry.second, ++diroffset);
    free(entry.first);
    free(entry.second);
    bufsize += entrysize;
  }

  fuse_reply_buf(req, buf, bufsize);
}

void FUSEOps::StatFs(fuse_req_t req, fuse_ino_t ino) {
  LOG(INFO) << "FUSEOps::StatFs " << ino;
  auto fs_info = (UserData *)fuse_req_userdata(req);
  auto partition = fs_info->disk->GetPartitionByInode(ino);

  struct statvfs stbuf;
  partition->StatFs(&stbuf);
  stbuf.f_files = 200;
  fuse_reply_statfs(req, &stbuf);
}

void FUSEOps::RetrieveReply(fuse_req_t req, void *cookie, fuse_ino_t ino,
                            off_t offset, struct fuse_bufvec *bufv) {
  LOG(INFO) << "FUSEOps::RetrieveReply";
  fuse_reply_none(req);
}

void FUSEOps::ForgetMulti(fuse_req_t req, size_t count,
                          struct fuse_forget_data *forgets) {
  LOG(INFO) << "FUSEOps::ForgetMulti";
  // Show yourself, coward; I will never forget Multi

  fuse_reply_none(req);
}

void FUSEOps::IOCtl(fuse_req_t req, fuse_ino_t ino, int cmd, void *arg,
                    struct fuse_file_info *fi, unsigned flags,
                    const void *in_buf, size_t in_bufsz, size_t out_bufsz) {
  LOG(INFO) << "FUSEOps::IOCtl " << cmd;
  /* XXX hoo boy */
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::WriteBuf(fuse_req_t req, fuse_ino_t ino, struct fuse_bufvec *bufv,
                       off_t off, struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::WriteBuf";
  /* XXX implement this */
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::ReadDirPlus(fuse_req_t req, fuse_ino_t ino, size_t size,
                          off_t off, struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::ReadDirPlus";
  /* XXX implement this */
  fuse_reply_err(req, ENOSYS);
}

/* ------------------------------- */
/* Here be unimplemented functions */
/* ------------------------------- */

void FUSEOps::SetXattr(fuse_req_t req, fuse_ino_t ino, const char *name,
                       const char *value, size_t size, int flags) {
  LOG(INFO) << "FUSEOps::SetXattr";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::GetXattr(fuse_req_t req, fuse_ino_t ino, const char *name,
                       size_t size) {
  LOG(INFO) << "FUSEOps::GetXattr";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::ListXattr(fuse_req_t req, fuse_ino_t ino, size_t size) {
  LOG(INFO) << "FUSEOps::ListXattr";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::RemoveXattr(fuse_req_t req, fuse_ino_t ino, const char *name) {
  LOG(INFO) << "FUSEOps::RemoveXattr";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::Access(fuse_req_t req, fuse_ino_t ino, int mask) {
  LOG(INFO) << "FUSEOps::Access";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::Bmap(fuse_req_t req, fuse_ino_t ino, size_t blocksize,
                   uint64_t idx) {
  LOG(INFO) << "FUSEOps::Bmap";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::FLock(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi,
                    int op) {
  LOG(INFO) << "FUSEOps::FLock";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::FAllocate(fuse_req_t req, fuse_ino_t ino, int mode, off_t offset,
                        off_t length, struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::FAllocate";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::Sync(fuse_req_t req, fuse_ino_t ino, int datasync,
                   struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::Sync";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::ReadLink(fuse_req_t req, fuse_ino_t ino) {
  LOG(INFO) << "FUSEOps::ReadLink";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::Symlink(fuse_req_t req, const char *link, fuse_ino_t parent,
                      const char *name) {
  LOG(INFO) << "FUSEOps::Symlink";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::Rename(fuse_req_t req, fuse_ino_t parent, const char *name,
                     fuse_ino_t newparent, const char *newname,
                     unsigned int flags) {
  LOG(INFO) << "FUSEOps::Rename";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::Link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent,
                   const char *newname) {
  LOG(INFO) << "FUSEOps::Link";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::SyncDir(fuse_req_t req, fuse_ino_t ino, int datasync,
                      struct fuse_file_info *fi) {
  LOG(INFO) << "FUSEOps::SyncDir";
  fuse_reply_err(req, ENOSYS);
}

void FUSEOps::Poll(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi,
                   struct fuse_pollhandle *ph) {
  LOG(INFO) << "FUSEOps::Poll";
  fuse_reply_err(req, ENOSYS);
}

const struct fuse_lowlevel_ops FUSEOps::GetLowlevelOps() {
  return {.init = &Init,
          .destroy = &Destroy,
          .lookup = &Lookup,
          .forget = &Forget,
          .getattr = &GetAttr,
          .setattr = &SetAttr,
          .readlink = &ReadLink,
          .mknod = &MkNod,
          .mkdir = &MkDir,
          .unlink = &Unlink,
          .rmdir = &RmDir,
          .symlink = &Symlink,
          .rename = &Rename,
          .link = &Link,
          .read = &Read,
          .write = &Write,
          .flush = &Flush,
          .fsync = &Sync,
          .readdir = &ReadDir,
          .fsyncdir = &SyncDir,
          .statfs = &StatFs,
          .setxattr = &SetXattr,
          .getxattr = &GetXattr,
          .listxattr = &ListXattr,
          .removexattr = &RemoveXattr,
          .access = &Access,
          .ioctl = &IOCtl,
          .poll = &Poll,
          .write_buf = &WriteBuf,
          .retrieve_reply = &RetrieveReply,
          .forget_multi = &ForgetMulti,
          .flock = &FLock,
          .fallocate = &FAllocate,
          .readdirplus = &ReadDirPlus};
}

}  // namespace Human68k
}  // namespace libFAT
