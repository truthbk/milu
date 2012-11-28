# compiler stuff
#if(APPLE)
#	set(CMAKE_C_COMPILER "/usr/bin/llvm-gcc-4.2")
#	set(CMAKE_CXX_COMPILER "/usr/bin/llvm-g++-4.2")
#endif(APPLE)

SET_TARGET_PROPERTIES( hmilu PROPERTIES COMPILE_FLAGS -fPIC )

add_executable(test_hash test_hash.c)
target_link_libraries(test_hash hmilu cunit)
