# ************ Client program ************

add_executable ( setbtmac setbtmac.c )

#include_directories (
#    ${CMAKE_SOURCE_DIR}/include
#)

target_link_libraries ( setbtmac ql_mcm_api )
