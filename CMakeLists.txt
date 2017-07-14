cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(CXXDecomQt)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)
find_package (HDF5 REQUIRED) # Find non-cmake built HDF5
find_package (Boost COMPONENTS iostreams REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(include)

file(GLOB SOURCES "src/*.cpp")


add_executable(${PROJECT_NAME} "include/backend.h" "qml.qrc" ${SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick hdf5 pthread ${Boost_LIBRARIES})
