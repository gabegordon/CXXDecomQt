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

find_package(HDF5 REQUIRED)

find_package(Qt5 COMPONENTS Core Gui Qml QuickControls2 Quick REQUIRED)
find_package(PkgConfig REQUIRED)
if(MINGW)
	pkg_check_modules(QT5ALL REQUIRED Qt5Core Qt5Gui Qt5Qml Qt5QuickControls2 Qt5Quick)
endif()

set(QT_LIBRARIES Qt5::Core Qt5::Gui Qt5::Qml Qt5::QuickControls2 Qt5::Quick)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package (Boost COMPONENTS iostreams REQUIRED)
include_directories(
  include
  ${Boost_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
)

set(DECOM_LINK_LIBS
	${Boost_LIBRARIES}
  ${HDF5_LIBRARIES}
	${QT_LIBRARIES}
)

set(QT5PLATFORMSUPPORT_LIBS
        Qt5FontDatabaseSupport::Qt5FontDatabaseSupport
        Qt5ThemeSupport::Qt5ThemeSupport
        Qt5EventDispatcherSupport::Qt5EventDispatcherSupport
)

if(MINGW)
  add_definitions(-DQT_STATICPLUGIN)
	list(APPEND DECOM_LINK_LIBS Qt5::QSvgPlugin)
	list(APPEND DECOM_LINK_LIBS Qt5::QWindowsIntegrationPlugin)
  list(APPEND DECOM_LINK_LIBS ${QTPLATFORMSUPPORT_LIBS})
	list(APPEND DECOM_LINK_LIBS ${QT5ALL_LDFLAGS})
endif()

file(GLOB SOURCES "src/[A-z]*.cpp")
file(GLOB INCLUDES "include/[A-z]*.h")

add_executable(${PROJECT_NAME} "qml.qrc" ${SOURCES} ${INCLUDES})

target_link_libraries(${PROJECT_NAME} ${DECOM_LINK_LIBS})

if(NOT UNIX AND NOT MINGW)
  add_custom_command(TARGET CXXDecomQt
                       PRE_BUILD
                       COMMAND cmd /c robocopy ../deps/ ./bin/ /s /e)
endif()
