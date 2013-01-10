cmake_minimum_required (VERSION 2.6)
project (milu)

# version
set (milu_VERSION_MAJOR 0)
set (milu_VERSION_MINOR 1)
set (milu_VERSION_PATCH 0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(CUnit)
if(CUNIT_FOUND)
	enable_testing()
endif(CUNIT_FOUND)

# headers for CMake settings
#configure_file (
#	"${PROJECT_SOURCE_DIR}/miluConfig.h.in"
#	"${PROJECT_BINARY_DIR}/miluConfig.h"
#	)

# include dirs
include_directories(${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/include/milu/
	${PROJECT_SOURCE_DIR}/include/milutil)

set(CMAKE_C_FLAGS "-Wall -W -fbuiltin")
set(CMAKE_CXX_FLAGS "-Wall -W -fbuiltin")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -gdwarf-2 -g3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2 -g3")

# compile as C99
add_definitions(-std=gnu99 -D_POOLING)

add_subdirectory(src)
if(CUNIT_FOUND)
	add_subdirectory(src/tests)
endif(CUNIT_FOUND)
