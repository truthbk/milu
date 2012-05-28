cmake_minimum_required (VERSION 2.6)
project (milu C)

add_subdirectory(src)

# version
set (milu_VERSION_MAJOR 0)
set (milu_VERSION_MINOR 1)

# headers for CMake settings
configure_file (
	"${PROJECT_SOURCE_DIR}/miluConfig.h.in"
	"${PROJECT_BINARY_DIR}/miluConfig.h"
	)

# include dirs
include_directories("${PROJECT_BINARY_DIR}")

# compiler stuff
set(CMAKE_CXX_FLAGS "-g -Wall")

# compile as C99
add_definitions(-std=c99)
