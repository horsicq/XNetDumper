cmake_minimum_required(VERSION 3.5)

project(xnetdumper VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql)

add_definitions(-DUSE_DEX)
add_definitions(-DUSE_PDF)
add_definitions(-DUSE_ARCHIVE)
add_definitions(-DUSE_YARA)
add_definitions(-DUSE_XPROCESS)

include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XProcessWidget/xprocesswidgetadvanced.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XAboutWidget/xaboutwidget.cmake)

set(PROJECT_SOURCES
        ${XPROCESSWIDGETADVANCED_SOURCES}
        ${XABOUTWIDGET_SOURCES}
        dialogabout.cpp
        dialogabout.ui
        dialogoptions.cpp
        dialogoptions.ui
        main_gui.cpp
        guimainwindow.cpp
        guimainwindow.ui
        rsrc.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(xnetdumper
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(WIN32)
        add_executable(xnetdumper WIN32
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(xnetdumper
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(xnetdumper PRIVATE bzip2)
target_link_libraries(xnetdumper PRIVATE lzma)
target_link_libraries(xnetdumper PRIVATE zlib)
target_link_libraries(xnetdumper PRIVATE capstone)
target_link_libraries(xnetdumper PRIVATE cppfilt)
target_link_libraries(xnetdumper PRIVATE yara)

target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Script)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::ScriptTools)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

if(WIN32)
    target_link_libraries(xnetdumper PRIVATE Wintrust)
    target_link_libraries(xnetdumper PRIVATE Crypt32)
endif()

include(GNUInstallDirs)

if(WIN32)
    install (TARGETS xnetdumper DESTINATION "./")
else()
    install (TARGETS xnetdumper DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(xnetdumper)
endif()
