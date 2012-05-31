add_library(hmilu miluhlib/hashtbl.c)
SET_TARGET_PROPERTIES( hmilu PROPERTIES COMPILE_FLAGS -fPIC )
add_library(milu SHARED milu/milu.c)
target_link_libraries(milu hmilu)

