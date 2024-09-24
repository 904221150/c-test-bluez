cmake_minimum_required (VERSION 2.8.1)

project(test-bluez)

set(CMAKE_VERBOSE_MAKEFILE off)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0")
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED gio-2.0)
pkg_search_module(GLIB2 REQUIRED dbus-1)
pkg_search_module(GLIB2 REQUIRED gobject-2.0)
pkg_search_module(GLIB2 REQUIRED glib-2.0)

add_subdirectory(gdbus)

INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/)
INCLUDE_DIRECTORIES(${CMAKE_PREFIX_PATH}/include/gio-unix-2.0)
link_directories(${GLIB2_LIBRARY_DIRS})

add_executable(test-bluez test-gdbus.c agent-server.c ${PROJECT_BINARY_DIR}/bluez-agent.c)
target_link_libraries(test-bluez ${GLIB2_LIBRARIES} pthread)






