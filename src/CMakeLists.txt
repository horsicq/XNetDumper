cmake_minimum_required(VERSION 3.10)

include(../dep/build_tools/cmake/deploy.cmake)
file (STRINGS ${PROJECT_SOURCE_DIR}/release_version.txt X_PROJECT_VERSION)

project(XNetDumper VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

#set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_OUTPUT_FILE_PREFIX packages)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "XNetDumper")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "XNetDumper")
set(CPACK_PACKAGE_VENDOR "ntinfo")
set(CPACK_PACKAGE_NAME "XNetDumper")
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/../LICENSE)
set(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/../README.md)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "horsicq@gmail.com")

include(CPack)

SET(X_MACOSX_BUNDLE_GUI_IDENTIFIER com.example.xnetdumper CACHE INTERNAL "X_MACOSX_BUNDLE_GUI_IDENTIFIER")

add_subdirectory(../dep/XCppfilt XCppfilt)
add_subdirectory(../dep/XCapstone XCapstone)
add_subdirectory(../dep/XArchive XArchive)
add_subdirectory(../dep/XYara XYara)
add_subdirectory(gui xnetdumper)

# TODO translation


