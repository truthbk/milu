add_library(libmilu miluhlib/hashtbl.c)
add_library(milu milu/milu.c)
target_link_libraries(milu libmilu)
