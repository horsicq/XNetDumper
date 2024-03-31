cmake_minimum_required(VERSION 3.10)
include(../dep/build_tools/cmake/deploy.cmake)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

if(${QT_VERSION_MAJOR} EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Network PrintSupport OpenGL Svg Sql Qml)
endif()

file (STRINGS ${PROJECT_SOURCE_DIR}/release_version.txt X_PROJECT_VERSION)
project(XNetDumper VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(X_COMPANYNAME "ntinfo")
set(X_PROJECTNAME "XNetDumper")
set(X_MAINTAINER "horsicq@gmail.com")
set(X_DESCRIPTION "XNetDumper is a dumper")
set(X_HOMEPAGE "http://ntinfo.biz")
set(X_ORIGINAL_FILENAME "xnetdumper")

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.xnetdumper)
endif()

deploy_init()

add_subdirectory(../dep/XCppfilt XCppfilt)
add_subdirectory(../dep/XCapstone XCapstone)
add_subdirectory(../dep/XArchive XArchive)
add_subdirectory(../dep/XYara XYara)
add_subdirectory(gui xnetdumper)

# TODO translation


