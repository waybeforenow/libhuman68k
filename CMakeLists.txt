cmake_minimum_required(VERSION 3.13.4)
project (libhuman68k-cc)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse3)

set(SOURCES
    BPB.cc
    Disk.cc
    FAT.cc
    Partition.cc
    Entity/File.cc
    Entity/Directory.cc
    Entity/Entity.cc
    Helpers/DirectoryHelper.cc
    Helpers/DiskHelper.cc
    Helpers/FileHelper.cc
    Helpers/PartitionHelper.cc
    FUSE/FUSEOps.cc
    lib/easyloggingpp/easylogging++.cc
   )
add_executable(main 
               ${SOURCES} Main.cc)
add_executable(fuse
               ${SOURCES} FUSE/Main.cc)

target_link_libraries(main -L/usr/local/lib ${FUSE_LIBRARIES})
target_include_directories(main PUBLIC ${FUSE_INCLUDE_DIRS})
target_compile_options(main PUBLIC ${FUSE_CFLAGS_OTHER})
target_link_libraries(fuse -L/usr/local/lib ${FUSE_LIBRARIES})
target_include_directories(fuse PUBLIC ${FUSE_INCLUDE_DIRS})
target_compile_options(fuse PUBLIC ${FUSE_CFLAGS_OTHER})
