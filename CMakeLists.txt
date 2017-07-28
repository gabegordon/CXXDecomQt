cmake_minimum_required(VERSION 2.8.12)

project(CXXDecomQt)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE release)
# Generate compile commands for IDE autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXECUTABLE_OUTPUT_PATH "bin")


set(QT5_PKGS Core Qml Gui Quick QuickControls2)

set(QT5_LIBS "")

#Find Qt Libs
foreach(pkg ${QT5_PKGS})
  find_package(Qt5${pkg} REQUIRED)
  list(APPEND QT5_LIBS Qt5::${pkg})
endforeach()

#Find HDF5 Library
find_package(HDF5 REQUIRED)

#Find Boost (static)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem iostreams REQUIRED)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra")

include_directories(
  include
  ${QT_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIR}
  ${QT5ALL_INCLUDE_DIRS}
  )

set(DECOM_LINK_LIBS
  ${Boost_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${QT_LIBRARIES}
  )

file(GLOB SOURCES "src/[A-z]*.cpp")
file(GLOB INCLUDES "include/[A-z]*.h")
add_executable(${PROJECT_NAME} ${EXE_TYPE} "qml.qrc" ${SOURCES} ${INCLUDES})

target_link_libraries(${PROJECT_NAME} ${LINKER_FLAGS} ${QT5_LIBS} ${DECOM_LINK_LIBS} pthread)
