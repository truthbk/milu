# include dirs
include_directories (
	${PROJECT_SRC_DIR}/include/milu/
	${PROJECT_SRC_DIR}/include/miluhlib/hashtbl
	${PROJECT_SRC_DIR}/include/miluhlib/hash
	${PROJECT_SRC_DIR}/include/miluhlib/list)

add_library(libmilu miluhlib/hashtbl.c)
add_library(milu milu/milu.c)
target_link_libraries(milu libmilu)
