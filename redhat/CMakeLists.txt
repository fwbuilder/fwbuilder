# Can't use PROJECT_VERSION; rpmbuild doesn't like hyphens in versions
set(RPM_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

if(PROJECT_VERSION_EXTRA)
 set(RPM_RELEASE "0${PROJECT_VERSION_EXTRA}")
 string(REPLACE "-" "." RPM_RELEASE "${RPM_RELEASE}")
else()
 set(RPM_RELEASE "1")
endif(PROJECT_VERSION_EXTRA)

configure_file("${CMAKE_SOURCE_DIR}/redhat/fwbuilder.spec.in" "${CMAKE_BINARY_DIR}/redhat/fwbuilder.spec" @ONLY)
configure_file("${CMAKE_SOURCE_DIR}/redhat/mktarball.sh.in" "${CMAKE_BINARY_DIR}/redhat/mktarball.sh" @ONLY)
