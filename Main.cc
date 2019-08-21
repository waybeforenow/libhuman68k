#include <stdio.h>

#include <memory>

#define FUSE_USE_VERSION 31
#include "FUSE/FUSEOps.h"
#include "fuse_lowlevel.h"
#include "lib/easyloggingpp/easylogging++.h"

INITIALIZE_EASYLOGGINGPP

void Usage() { fprintf(stderr, "meep!~\n"); }

int main(int argc, char** argv) {
  START_EASYLOGGINGPP(argc, argv);

  if (argc < 2) {
    Usage();
    return 1;
  }

  char* filename = argv[1];

  struct fuse_conn_info conn;
  libFAT::Human68k::FUSEOps::UserData userdata = {filename, nullptr};
  libFAT::Human68k::FUSEOps::Init(&userdata, &conn);
  auto dir = userdata.disk->GetPartitionByInode(0)->GetDirectory(0);
  auto entries = dir->GetEntries(10, 0);
  for (const auto& entry : *entries) {
    LOG(INFO) << "Entry: " << entry.first << " (" << entry.second << ")";
  }

  return 0;
}

