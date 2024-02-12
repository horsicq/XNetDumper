cmake_minimum_required(VERSION 3.5)

project(gui VERSION 0.1 LANGUAGES CXX)

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

set(PROJECT_SOURCES
        ${XPROCESSWIDGETADVANCED_SOURCES}
        main_gui.cpp
        guimainwindow.cpp
        guimainwindow.h
        guimainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(gui SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(gui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(gui PRIVATE bzip2)
target_link_libraries(gui PRIVATE lzma)
target_link_libraries(gui PRIVATE zlib)
target_link_libraries(gui PRIVATE capstone)
target_link_libraries(gui PRIVATE cppfilt)
target_link_libraries(gui PRIVATE yara)

target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Script)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::ScriptTools)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

if(WIN32)
    target_link_libraries(gui PRIVATE Wintrust)
    target_link_libraries(gui PRIVATE Crypt32)
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gui)
endif()
set_target_properties(gui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gui)
endif()
