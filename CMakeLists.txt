cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
project(CXXDecomQt C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -pthread")
set(CMAKE_BUILD_TYPE release)
set(EXECUTABLE_OUTPUT_PATH "bin")

find_package (Threads)

find_package(HDF5 REQUIRED)

find_package(Qt5 COMPONENTS Core Quick Qml Svg Network REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(QT5SVG Qt5Svg)
link_directories(${QT5SVG_LIBRARY_DIRS})
pkg_check_modules(QT5NETWORK Qt5Network)
link_directories(${QT5NETWORK_LIBRARY_DIRS})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package (Boost COMPONENTS iostreams REQUIRED)

include_directories(include)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIR})

file(GLOB SOURCES "src/[A-z]*.cpp")
file(GLOB INCLUDES "include/[A-z]*.h")

add_executable(${PROJECT_NAME} "qml.qrc" ${SOURCES} ${INCLUDES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick Qt5::Qml Qt5::Svg Qt5::Network pthread ${QT5SVG_LIBRARIES} ${QT5NETWORK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} 
${Boost_LIBRARIES} 
${HDF5_LIBRARIES})

if(NOT UNIX)
if(NOT MINGW)
  add_custom_command(TARGET CXXDecomQt
                       PRE_BUILD
                       COMMAND cmd /c robocopy ../deps/ ./bin/ /s /e)
endif()
endif()
