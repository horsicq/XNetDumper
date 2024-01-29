cmake_minimum_required(VERSION 3.10)

project(XNetDumper VERSION 0.1 LANGUAGES CXX)

# Set up project options
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt components
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Concurrent Core Qml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Concurrent Core Qml)

# Include external CMake files
file(GLOB MY_DIRECTORY_FILES "${PROJECT_SOURCE_DIR}/../dep/die_script/*.cmake")
foreach(FILE ${MY_DIRECTORY_FILES})
    include(${FILE})
endforeach()

# Source files
file(GLOB_RECURSE PROJECT_SOURCES
    "../dep/die_script/*.cpp"
    "../dep/Formats/*.cpp"
    "../dep/XArchive/*.cpp"
    "../dep/XCapstone/*.cpp"
    "../dep/XDEX/*.cpp"
    "../dep/XPDF/*.cpp"
    main.cpp
    XNetDumper.cpp
    XNetDumper.h
    XNetDumper.ui
    DLLInfoDialog.cpp
    DLLInfoDialog.h
    LoadAssembly.h
    searchdialog.cpp
    searchdialog.h
    updater.cpp
    updater.h
    updater.ui
    loadedmodulesdialog.h
    loadedmodulesdialog.cpp
    memorymodificationdialog.cpp
    memorymodificationdialog.h
)

# Create executable or library based on Qt version
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(XNetDumper MANUAL_FINALIZATION ${PROJECT_SOURCES} LoadAssembly.cpp
        memorymodificationdialog.h memorymodificationdialog.cpp memorymodificationdialog.ui
    )
else()
    if(ANDROID)
        add_library(XNetDumper SHARED ${PROJECT_SOURCES})
    else()
        add_executable(XNetDumper ${PROJECT_SOURCES})
    endif()
endif()

# Finalize Qt executable if using Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(XNetDumper)
endif()

# Link libraries
target_link_libraries(XNetDumper PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Qml
    bzip2
    lzma
    zlib
    Crypt32
    Wintrust
    capstone
)
