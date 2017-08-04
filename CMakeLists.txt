cmake_minimum_required(VERSION 2.8.12)

project(CXXDecomQt)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra -Ofast -march=native")
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

include_directories(
  include
  ${QT_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIR}
  )

set(DECOM_LINK_LIBS
  ${Boost_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${QT_LIBRARIES}
  )

file(GLOB SOURCES "src/[A-z]*.cpp")
file(GLOB INCLUDES "include/[A-z]*.hpp")
add_executable(${PROJECT_NAME} "qml.qrc" ${SOURCES} ${INCLUDES})

target_link_libraries(${PROJECT_NAME} ${QT5_LIBS} ${DECOM_LINK_LIBS} pthread)

option(BUILD_TESTS "Determines whether to build tests." ON)
if(BUILD_TESTS)

  set(DECOM_TEST_SOURCES
    tests/TestDecom.cpp
    )

  enable_testing()
  # Prepare "Catch" library for other executables
  set(CATCH_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include/catch)
  add_library(Catch INTERFACE)
  target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

  add_executable(Decom_Test ${DECOM_TEST_SOURCES})
  target_link_libraries(Decom_Test ${DECOM_LINK_LIBS} Catch pthread)
  add_test(NAME TestDecom COMMAND Decom_Test)

endif()
