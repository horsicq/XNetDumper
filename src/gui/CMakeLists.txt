cmake_minimum_required(VERSION 3.5)

include(../../dep/build_tools/cmake/deploy.cmake)
include(GNUInstallDirs)

# X_RESOURCES: where runtime data has to go so XOptions::convertPathName("$data/...")
# can find it again. This project did not pull the shared rule in, and installed to
# CMAKE_INSTALL_LIBDIR - the Debian multiarch lib/x86_64-linux-gnu, which is not on
# the list convertPathName() searches.
include(../../dep/build_tools/cmake/x_data_destination.cmake)
if(NOT DEFINED X_RESOURCES)
    x_data_install_destination(X_RESOURCES)
endif()

project(xnetdumper VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
target_link_libraries(xnetdumper PRIVATE ppmd)
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

if(WIN32)
    install (TARGETS xnetdumper DESTINATION "./")
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XInfoDB/info DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XStyles/qss DESTINATION "./" OPTIONAL)
    install (DIRECTORY ../../dep/XYara/yara_rules DESTINATION "./" OPTIONAL)
    install (FILES ../../dep/signatures/crypto.db DESTINATION "./signatures" OPTIONAL)
    deploy_qt()
    deploy_msvc()
else()
    install (TARGETS xnetdumper DESTINATION ${CMAKE_INSTALL_BINDIR})
    install (DIRECTORY ../../dep/Detect-It-Easy/db DESTINATION "${X_RESOURCES}" OPTIONAL)
    install (DIRECTORY ../../dep/Detect-It-Easy/db_custom DESTINATION "${X_RESOURCES}" OPTIONAL)
    install (DIRECTORY ../../dep/XInfoDB/info DESTINATION "${X_RESOURCES}" OPTIONAL)
    install (DIRECTORY ../../dep/XStyles/qss DESTINATION "${X_RESOURCES}" OPTIONAL)
    install (DIRECTORY ../../dep/XYara/yara_rules DESTINATION "${X_RESOURCES}" OPTIONAL)
    install (FILES ../../dep/signatures/crypto.db DESTINATION "${X_RESOURCES}/signatures" OPTIONAL)
endif()

# if(${QT_VERSION_MAJOR} EQUAL 6)
#     qt_finalize_executable(xnetdumper)
# endif()
