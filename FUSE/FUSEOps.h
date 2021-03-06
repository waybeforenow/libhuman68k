#ifndef LIBFAT_HUMAN68K_FUSEOPS_H_
#define LIBFAT_HUMAN68K_FUSEOPS_H_

#include <functional>
#include <memory>

#include "../Disk.h"

#define FUSE_USE_VERSION 31
#include "fuse_lowlevel.h"

namespace libFAT {
namespace Human68k {

/**
 * @class FUSEOps fuseops.h
 * @brief Implements the FUSE API using libFAT::Human68k.
 */
class FUSEOps {
 public:
  /**
   * @brief Struct for holding data internally.
   */
  struct UserData {
    char *file_name;
    std::unique_ptr<Disk> disk;
  };

  /**
   * @brief Returns a struct of the low-level FUSE API callbacks.
   */
  const static struct fuse_lowlevel_ops GetLowlevelOps();

  /**
   * @brief Initializes a Disk from the supplied filename.
   * Places it into the UserData.
   */
  static void Init(void *userdata, struct fuse_conn_info *conn);

  /**
   * @brief Destroys the Disk object so the program can exit cleanly.
   */
  static void Destroy(void *userdata);

  /**
   * @brief Performs a lookup of a File by its filename and parent directory.
   * @returns the inode and file attributes.
   */
  static void Lookup(fuse_req_t req, fuse_ino_t parent, const char *name);

  static void Forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup);

  static void GetAttr(fuse_req_t req, fuse_ino_t ino,
                      struct fuse_file_info *fi);

  static void SetAttr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
                      int to_set, struct fuse_file_info *fi);

  static void MkNod(fuse_req_t req, fuse_ino_t parent, const char *name,
                    mode_t mode, dev_t rdev);

  static void MkDir(fuse_req_t req, fuse_ino_t parent, const char *name,
                    mode_t mode);

  static void Unlink(fuse_req_t req, fuse_ino_t parent, const char *name);

  static void RmDir(fuse_req_t req, fuse_ino_t parent, const char *name);

  static void Read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                   struct fuse_file_info *fi);

  static void Write(fuse_req_t req, fuse_ino_t ino, const char *buf,
                    size_t size, off_t off, struct fuse_file_info *fi);

  static void Flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);

  static void ReadDir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                      struct fuse_file_info *fi);

  static void StatFs(fuse_req_t req, fuse_ino_t ino);

  static void IOCtl(fuse_req_t req, fuse_ino_t ino, int cmd, void *arg,
                    struct fuse_file_info *fi, unsigned flags,
                    const void *in_buf, size_t in_bufsz, size_t out_bufsz);

  static void WriteBuf(fuse_req_t req, fuse_ino_t ino, struct fuse_bufvec *bufv,
                       off_t off, struct fuse_file_info *fi);

  static void RetrieveReply(fuse_req_t req, void *cookie, fuse_ino_t ino,
                            off_t offset, struct fuse_bufvec *bufv);

  static void ForgetMulti(fuse_req_t req, size_t count,
                          struct fuse_forget_data *forgets);

  static void ReadDirPlus(fuse_req_t req, fuse_ino_t ino, size_t size,
                          off_t off, struct fuse_file_info *fi);
};

}  // namespace Human68k
}  // namespace libFAT

#endif  // LIBFAT_HUMAN68K_FUSEOPS_H_
