find_package(PkgConfig)

set(INCLUDE_PATH "${ACL_SOURCE_DIR}/include"
	 "${AIP_SOURCE_DIR}/include"
	 "${ADSL_SOURCE_DIR}/include"
	 "${AFML_SOURCE_DIR}/include"
	 "${AVSCommon_SOURCE_DIR}/include"
	 "${QSTHWManager_SOURCE_DIR}/include"
	 "${MediaPlayer_SOURCE_DIR}/include"
	 "${SYSROOT_DIR}/usr/include/mm-audio/qsthw_api"
	 "${SYSROOT_DIR}/usr/include/system"
         "${GST_INCLUDE_DIRS}"
   )

set(LINK_PATH AVSCommon ACL AuthDelegate ADSL AFML AIP qtisoundtriggerhw DefaultClient)

set(libName QSTHWManager)

file(GLOB_RECURSE capSourceFiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp")
add_library(${libName} SHARED ${capSourceFiles})
target_include_directories(${libName} PUBLIC "${INCLUDE_PATH}")
target_link_libraries(${libName} "${LINK_PATH}" cutils)

INSTALL(TARGETS QSTHWManager DESTINATION lib)
