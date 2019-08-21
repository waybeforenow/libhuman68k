#include "../lib/easyloggingpp/easylogging++.h"
#include "FUSEOps.h"

INITIALIZE_EASYLOGGINGPP

using namespace libFAT::Human68k;

/**
 * Construct a FUSEOps::UserData from the arguments passed to the program.
 *
 * @param data  a pointer to a valid initialized FUSEOps::UserData
 * @param arg   the string to parse (as a key=value pair)
 */
int parse_user_data(void *data, const char *arg, int key,
                    struct fuse_args *outargs) {
  if (strncmp("loop=", arg, 5) == 0) {
    struct FUSEOps::UserData *user_data = (struct FUSEOps::UserData *)data;
    user_data->file_name = strdup(arg + 5);
    return 0;
  }

  return 1;
}

int main(int argc, char *argv[]) {
  START_EASYLOGGINGPP(argc, argv);

  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  struct fuse_session *se;
  struct fuse_cmdline_opts opts;
  int ret = -1;
  struct FUSEOps::UserData user_data;
  if (fuse_opt_parse(&args, (void *)(&user_data), NULL, &parse_user_data) != 0)
    return 1;
  if (fuse_parse_cmdline(&args, &opts) != 0) return 1;
  if (opts.show_help) {
    printf("usage: %s [options] <mountpoint>\n\n", argv[0]);
    fuse_cmdline_help();
    fuse_lowlevel_help();
    return 0;
  } else if (opts.show_version) {
    printf("FUSE library version %s\n", fuse_pkgversion());
    fuse_lowlevel_version();
    return 0;
  }
  if (opts.mountpoint == NULL) {
    printf("usage: %s [options] <mountpoint>\n", argv[0]);
    printf("       %s --help\n", argv[0]);
    return 1;
  }

  struct fuse_lowlevel_ops ll_ops = FUSEOps::GetLowlevelOps();
  se = fuse_session_new(&args, &ll_ops, sizeof(ll_ops), &user_data);
  if (se == NULL) {
    free(opts.mountpoint);
    fuse_opt_free_args(&args);
    return 1;
  }
  if (fuse_set_signal_handlers(se) != 0) {
    fuse_session_destroy(se);
    free(opts.mountpoint);
    fuse_opt_free_args(&args);
    return 1;
  }
  if (fuse_session_mount(se, opts.mountpoint) != 0) {
    fuse_remove_signal_handlers(se);
    fuse_session_destroy(se);
    free(opts.mountpoint);
    fuse_opt_free_args(&args);
    return 1;
  }

  // fuse_daemonize(opts.foreground);
  /* Block until ctrl+c or fusermount -u */
  if (opts.singlethread) {
    ret = fuse_session_loop(se);
  } else {
    ret = fuse_session_loop_mt(se, opts.clone_fd);
  }
  fuse_session_unmount(se);
  fuse_remove_signal_handlers(se);
  fuse_session_destroy(se);
  free(opts.mountpoint);
  fuse_opt_free_args(&args);
  return ret ? 1 : 0;
}

