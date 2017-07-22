cmake_minimum_required(VERSION 2.8.12)

project(CXXDecomQt)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
# Generate compile commands for IDE autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OUTPUT_PATH "bin")

if(MINGW)
  option(STATIC_QT "Use static Qt5 libraries" ON)
endif()

if(STATIC_QT)
  set(LINKER_FLAGS -static -static-libstdc++ -static-libgcc)
  find_package(Qt5Core REQUIRED)
  find_package(PkgConfig REQUIRED)
  set(QT_PREFIX Qt5)
endif()

set(QT5_PKGS Core Qml Gui WinExtras Quick QuickControls2)

set(QT5_LIBS "")

#Find Qt Libs
foreach(pkg ${QT5_PKGS})
  find_package(${QT_PREFIX}${pkg} REQUIRED)
  list(APPEND QT5_LIBS ${QT_PREFIX}::${pkg})
  pkg_check_modules(QT5ALL Qt5Core Qt5Qml Qt5Gui Qt5WinExtras Qt5Quick Qt5QuickControls2)
endforeach()

#Get platform support libs
if(MINGW)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(Qt5ThemeSupport REQUIRED)
find_package(Qt5FontDatabaseSupport REQUIRED)
find_package(Qt5EventDispatcherSupport REQUIRED)
find_library(Qt5FbSupport_LIB libQt5FbSupport REQUIRED)
find_library(Qt5PlatformCompositorSupport_LIB libQt5AccessibilitySupport)
endif()
#Find HDF5 Library
find_package(HDF5 REQUIRED)

#Find Boost (static)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS iostreams REQUIRED)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra")

if(WIN32)
  set(EXE_TYPE WIN32)
endif()

include_directories(
  include
  ${QT_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${HDF5_INCLUDE_DIR}
  ${QT5ALL_INCLUDE_DIRS}
  ${Qt5Core_PRIVATE_INCLUDE_DIRS}
  ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
  )

set(DECOM_LINK_LIBS
  ${BOOST_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${QT_LIBRARIES}
  ${QT5ALL_LIBRARIES}
  )

set(QT5PLATFORMSUPPORT_LIBS
  Qt5FontDatabaseSupport::Qt5FontDatabaseSupport
  Qt5ThemeSupport::Qt5ThemeSupport
  Qt5EventDispatcherSupport::Qt5EventDispatcherSupport
  ${Qt5FbSupport_LIB}
  ${Qt5PlatformCompositorSupport_LIB}
  )

if(STATIC_QT)
  list(APPEND DECOM_LINK_LIBS Qt5::QWindowsIntegrationPlugin)
  list(APPEND DECOM_LINK_LIBS ${QT5ALL_LDFLAGS})
  list(APPEND DECOM_LINK_LIBS ${QT5PLATFORMSUPPORT_LIBS})
endif()

file(GLOB SOURCES "src/[A-z]*.cpp")
file(GLOB INCLUDES "include/[A-z]*.h")
add_executable(${PROJECT_NAME} ${EXE_TYPE} "qml.qrc" ${SOURCES} ${INCLUDES})

# Add the Qt5 Widgets for linking
target_link_libraries(${PROJECT_NAME} ${LINKER_FLAGS} ${QT5_LIBS} ${DECOM_LINK_LIBS})
