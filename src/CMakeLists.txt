# compiler stuff
#if(APPLE)
#	set(CMAKE_C_COMPILER "/usr/bin/llvm-gcc-4.2")
#	set(CMAKE_CXX_COMPILER "/usr/bin/llvm-g++-4.2")
#endif(APPLE)

add_library(hmilu milutil/hashtbl.c)
SET_TARGET_PROPERTIES( hmilu PROPERTIES COMPILE_FLAGS -fPIC )
add_library(milu SHARED milu/milu.c)
target_link_libraries(milu hmilu m)

