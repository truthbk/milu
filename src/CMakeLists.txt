# headers for CMake settings
configure_file (
	config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

# include dirs
include_directories (
	${PROJECT_SRC_DIR}/include/miluhlib/hashtbl
	${PROJECT_SRC_DIR}/include/miluhlib/hash
	${PROJECT_SRC_DIR}/include/miluhlib/list)

