cmake_minimum_required (VERSION 2.6)
project (milu C)

# version
set (milu_VERSION_MAJOR 0)
set (milu_VERSION_MINOR 1)
set (milu_VERSION_PATCH 0)

# headers for CMake settings
#configure_file (
#	"${PROJECT_SOURCE_DIR}/miluConfig.h.in"
#	"${PROJECT_BINARY_DIR}/miluConfig.h"
#	)

# include dirs
include_directories(${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/include/milu/
	${PROJECT_SOURCE_DIR}/include/miluhlib)

add_subdirectory(src)

# compiler stuff
if(APPLE)
	set(CMAKE_C_COMPILER "gcc-4.2")
	set(CMAKE_CXX_COMPILER "c++-4.2")
endif(APPLE)
set(CMAKE_CXX_FLAGS "-ggdb -Wall -W")

# compile as C99
add_definitions(-std=c99)
