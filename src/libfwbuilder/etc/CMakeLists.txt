project(etc)
configure_file(fwbuilder.dtd.in fwbuilder.dtd)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fwbuilder.dtd DESTINATION ${FWB_INSTALL_DATADIR})

