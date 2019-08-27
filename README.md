# `libfat-human68k`
FAT driver for Human68k-readable filesystems (written from the ground up).

## How to use
1. Install `fuse3`
2. `git clone https://github.com/waybeforenow/libhuman68k.git`
3. `mkdir build && cd build && mkdir mnt`
4. `cmake .. && make fuse`
5. `./fuse loop=[disk image file] mnt/`

## What works?
Directory traversal and reading file contents

## What doesn't work yet?
Creating directories, creating files, writing to files.
