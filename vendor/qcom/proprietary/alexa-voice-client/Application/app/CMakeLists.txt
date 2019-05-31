find_package(GTest ${GTEST_PACKAGE_CONFIG})

set(INCLUDE_PATH "${AVSManager_SOURCE_DIR}/include")
set(LINK_PATH AVSManager)

file(GLOB_RECURSE testSourceFiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Alexa*.cpp")
foreach (testSourceFile IN LISTS testSourceFiles)
    get_filename_component(testName ${testSourceFile} NAME_WE)
    add_executable(${testName} ${testSourceFile})
    target_include_directories(${testName} PUBLIC "${INCLUDE_PATH}")
    target_link_libraries(${testName} "${LINK_PATH}")
endforeach()

include(${Application_BINARY_DIR}/CTestCustom.cmake OPTIONAL)
separate_arguments(CTEST_CUSTOM_PRE_TEST UNIX_COMMAND "${CTEST_CUSTOM_PRE_TEST}")
separate_arguments(CTEST_CUSTOM_POST_TEST UNIX_COMMAND "${CTEST_CUSTOM_POST_TEST}")
add_custom_target(application
    COMMAND ${CTEST_CUSTOM_PRE_TEST}
    COMMAND AlexaClientApp
    COMMAND ${CTEST_CUSTOM_POST_TEST})

INSTALL(TARGETS AlexaClientApp DESTINATION bin)
