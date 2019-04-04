# Cmake file for building the main GUI executable
#
# Copyright Igor Siemienowicz 2018
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)

# Qt Stuff
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB UI_FILES "*.ui")
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

# GUI executable
file (GLOB_RECURSE GUI_SRC *.cpp *.rc *.qrc)
add_executable($ENV{QPRJ_PROJECT_NAME}-gui ${GUI_SRC} ${UI_HEADERS})
target_link_libraries($ENV{QPRJ_PROJECT_NAME}-gui
    Qt5::Widgets
    ${CONAN_LIBS}
    $ENV{QPRJ_PROJECT_NAME}-api
)

# If we're running under windows, we need the Qt DLLs with our GUI executable
if (WIN32)
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Widgets.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Concurrent.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Core.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5DBus.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Gui.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Network.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5OpenGL.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5PrintSupport.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Sql.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Test.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Widgets.dll
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CONAN_BIN_DIRS_QT}/Qt5Xml.dll    
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
