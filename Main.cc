#include <stdio.h>

#include <memory>

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

  return 0;
}

