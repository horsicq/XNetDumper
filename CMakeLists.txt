cmake_minimum_required(VERSION 3.10)

project(XNetDumper)

add_subdirectory(gui)

add_subdirectory(dep/XCapstone/)
add_subdirectory(dep/XArchive/3rdparty/bzip2)
add_subdirectory(dep/XArchive/3rdparty/lzma)
add_subdirectory(dep/XArchive/3rdparty/zlib)
