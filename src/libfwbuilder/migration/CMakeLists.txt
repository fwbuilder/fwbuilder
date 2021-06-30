FILE(GLOB migration_files "${CMAKE_CURRENT_SOURCE_DIR}/*.xslt")
install(FILES ${migration_files} DESTINATION ${FWB_INSTALL_DATADIR}/migration)

