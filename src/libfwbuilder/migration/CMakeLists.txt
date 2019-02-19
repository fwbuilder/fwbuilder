project(migration)
FILE(GLOB migration_files "${PROJECT_SOURCE_DIR}/*.xslt")
install(FILES ${migration_files} DESTINATION ${FWB_INSTALL_DATADIR}/migration)

