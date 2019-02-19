project(doc)

FILE(GLOB readme_files "${PROJECT_SOURCE_DIR}/README.*")

install(FILES
        AUTHORS
        ChangeLog
        COPYING
        Credits
        FWBuilder-Routing-LICENSE.txt
        PatchAcceptancePolicy.txt
	${readme_files}
	DESTINATION ${FWB_INSTALL_DOCDIR})

if(UNIX AND NOT APPLE)
    FIND_PROGRAM(GZIP_TOOL
    	NAMES gzip
    	PATHS /bin
    	      /usr/bin
    	      /usr/local/bin)
    
    IF(NOT GZIP_TOOL)
        MESSAGE(FATAL_ERROR "Could not find gzip for man page compression.")
    endif(NOT GZIP_TOOL)
    
    add_custom_target(generate_man ALL
        COMMAND ${GZIP_TOOL} -c ${CMAKE_CURRENT_SOURCE_DIR}/fwbedit.1 > ${CMAKE_BINARY_DIR}/fwbedit.1.gz
        COMMAND ${GZIP_TOOL} -c ${CMAKE_CURRENT_SOURCE_DIR}/fwbuilder.1 > ${CMAKE_BINARY_DIR}/fwbuilder.1.gz
        COMMAND ${GZIP_TOOL} -c ${CMAKE_CURRENT_SOURCE_DIR}/fwb_iosacl.1 > ${CMAKE_BINARY_DIR}/fwb_iosacl.1.gz
        COMMAND ${GZIP_TOOL} -c ${CMAKE_CURRENT_SOURCE_DIR}/fwb_ipf.1 > ${CMAKE_BINARY_DIR}/fwb_ipf.1.gz
        COMMAND ${GZIP_TOOL} -c ${CMAKE_CURRENT_SOURCE_DIR}/fwb_ipfw.1 > ${CMAKE_BINARY_DIR}/fwb_ipfw.1.gz
        COMMAND ${GZIP_TOOL} -c ${CMAKE_CURRENT_SOURCE_DIR}/fwb_ipt.1 > ${CMAKE_BINARY_DIR}/fwb_ipt.1.gz
        COMMAND ${GZIP_TOOL} -c ${CMAKE_CURRENT_SOURCE_DIR}/fwb_pf.1 > ${CMAKE_BINARY_DIR}/fwb_pf.1.gz
        COMMAND ${GZIP_TOOL} -c ${CMAKE_CURRENT_SOURCE_DIR}/fwb_pix.1 > ${CMAKE_BINARY_DIR}/fwb_pix.1.gz)
    
    install(FILES ${CMAKE_BINARY_DIR}/fwbedit.1.gz
            ${CMAKE_BINARY_DIR}/fwbuilder.1.gz
            ${CMAKE_BINARY_DIR}/fwb_iosacl.1.gz
            ${CMAKE_BINARY_DIR}/fwb_ipf.1.gz
            ${CMAKE_BINARY_DIR}/fwb_ipfw.1.gz
            ${CMAKE_BINARY_DIR}/fwb_ipt.1.gz
            ${CMAKE_BINARY_DIR}/fwb_pf.1.gz
            ${CMAKE_BINARY_DIR}/fwb_pix.1.gz
            DESTINATION ${CMAKE_INSTALL_DATADIR}/man/man1)
endif(UNIX AND NOT APPLE)

