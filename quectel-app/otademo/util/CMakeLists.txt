# ************ Client program ************

add_executable ( ota-test ota-test.c )

include_directories (
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries ( ota-test ql_ota )
