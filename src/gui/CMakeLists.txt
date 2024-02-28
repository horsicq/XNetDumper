cmake_minimum_required(VERSION 3.5)

include(../../dep/build_tools/cmake/deploy.cmake)

project(xnetdumper VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql)

if(${QT_VERSION_MAJOR} EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS Widgets Concurrent Network Script ScriptTools PrintSupport OpenGL Svg Sql)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Network PrintSupport OpenGL Svg Sql Qml)
endif()

message(STATUS ${QT_VERSION_MAJOR})

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
        dialogabout.h
        dialogabout.ui
        dialogoptions.cpp
        dialogoptions.h
        dialogoptions.ui
        main_gui.cpp
        guimainwindow.cpp
        guimainwindow.h
        guimainwindow.ui
        rsrc.qrc
)

if(WIN32)
    add_executable(xnetdumper WIN32
        ${PROJECT_SOURCES}
    )
else()
    add_executable(xnetdumper
        ${PROJECT_SOURCES}
    )
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
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

if(${QT_VERSION_MAJOR} EQUAL 5)
    target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Script)
    target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::ScriptTools)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(xnetdumper PRIVATE Qt${QT_VERSION_MAJOR}::Qml)
endif()

if(WIN32)
    target_link_libraries(xnetdumper PRIVATE Wintrust)
    target_link_libraries(xnetdumper PRIVATE Crypt32)
endif()

include(GNUInstallDirs)

if(WIN32)
    install (TARGETS xnetdumper DESTINATION "./")
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XInfoDB/info DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XStyles/qss DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XYara/yara_rules DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "./" OPTIONAL)
    install (FILES ../../dep/signatures/crypto.db DESTINATION "./signatures" OPTIONAL)
    deploy_qt()
    deploy_msvc()
else()
    install (TARGETS xnetdumper DESTINATION ${CMAKE_INSTALL_BINDIR})
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION ${CMAKE_INSTALL_LIBDIR}/xnetdumper OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION ${CMAKE_INSTALL_LIBDIR}/xnetdumper OPTIONAL)
    install (DIRECTORY ../../dep/XInfoDB/info DESTINATION ${CMAKE_INSTALL_LIBDIR}/xnetdumper OPTIONAL)
    install (DIRECTORY ../../dep/XStyles/qss DESTINATION ${CMAKE_INSTALL_LIBDIR}/xnetdumper OPTIONAL)
    install (DIRECTORY ../../dep/XYara/yara_rules DESTINATION ${CMAKE_INSTALL_LIBDIR}/xnetdumper OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION ${CMAKE_INSTALL_LIBDIR}/xnetdumper OPTIONAL)
    install (FILES ../../dep/signatures/crypto.db DESTINATION ${CMAKE_INSTALL_LIBDIR}/xnetdumper/signatures OPTIONAL)
endif()

# if(${QT_VERSION_MAJOR} EQUAL 6)
#     qt_finalize_executable(xnetdumper)
# endif()
