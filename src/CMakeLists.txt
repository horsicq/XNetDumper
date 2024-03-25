cmake_minimum_required(VERSION 3.10)
include(../dep/build_tools/cmake/deploy.cmake)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql)

if(${QT_VERSION_MAJOR} EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Network PrintSupport OpenGL Svg Sql Qml)
endif()

file (STRINGS ${PROJECT_SOURCE_DIR}/release_version.txt X_PROJECT_VERSION)

project(XNetDumper VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(CPACK_PACKAGE_INSTALL_DIRECTORY "XNetDumper")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "XNetDumper")
set(CPACK_PACKAGE_VENDOR "ntinfo")
set(CPACK_PACKAGE_NAME "XNetDumper")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "horsicq@gmail.com")

set(X_MACOSX_BUNDLE_GUI_IDENTIFIER com.example.xnetdumper CACHE INTERNAL "X_MACOSX_BUNDLE_GUI_IDENTIFIER")

deploy_init()

add_subdirectory(../dep/XCppfilt XCppfilt)
add_subdirectory(../dep/XCapstone XCapstone)
add_subdirectory(../dep/XArchive XArchive)
add_subdirectory(../dep/XYara XYara)
add_subdirectory(gui xnetdumper)

# TODO translation


