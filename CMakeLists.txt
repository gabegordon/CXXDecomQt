cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(CXXDecomQt)

set(CMAKE_CXX_STANDARD 14)

IF(WIN64)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ELSE(WIN64)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ENDIF(WIN64)

find_package(Qt5 COMPONENTS Core Quick Qml REQUIRED)

set(CTEST_BUILD_CONFIGURATION "Release")
set(BUILD_STATIC_EXECS ON)
find_package (HDF5 REQUIRED) # Find non-cmake built HDF5

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package (Boost COMPONENTS iostreams REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIR})
include_directories(include)
include_directories(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} "include/backend.h" "qml.qrc" ${SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick Qt5::Qml pthread ${Boost_LIBRARIES} ${QT_DEFINITIONS} ${HDF5_LIBRARIES})
