cmake_minimum_required(VERSION 3.10)

# project() MUST precede find_package(Qt...) -- before it no language is enabled,
# Qt5's imported targets are created STATIC, and CMake then links them through
# IMPORTED_LOCATION (bin\Qt5*.dll) instead of IMPORTED_IMPLIB (lib\Qt5*.lib),
# producing LNK1107 on the .dll. ${PROJECT_SOURCE_DIR} is unusable for the version
# file (empty pre-project(), src/ afterwards), so address it relative to this list
# file, which is the repo root standalone and from the root build alike.
file(STRINGS ${CMAKE_CURRENT_LIST_DIR}/../release_version.txt X_PROJECT_VERSION)

project(XNetDumper VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

include(../dep/build_tools/cmake/deploy.cmake)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

if(${QT_VERSION_MAJOR} EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Network PrintSupport OpenGL Svg Sql Qml)
endif()


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


