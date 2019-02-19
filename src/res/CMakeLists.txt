project(res)
configure_file(objects_init.xml.in objects_init.xml)
configure_file(templates.xml.in templates.xml)
FILE(GLOB os_files "${PROJECT_SOURCE_DIR}/os/*.xml")
FILE(GLOB platform_files "${PROJECT_SOURCE_DIR}/platform/*.xml")
FILE(GLOB help_files "${PROJECT_SOURCE_DIR}/help/en_US/*.html" "${PROJECT_SOURCE_DIR}/help/en_US/*.png" "${PROJECT_SOURCE_DIR}/help/en_US/*.jpg")

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/objects_init.xml
	${CMAKE_CURRENT_BINARY_DIR}/templates.xml
	resources.xml
	DESTINATION ${FWB_INSTALL_DATADIR})
install(FILES ${os_files} DESTINATION ${FWB_INSTALL_DATADIR}/os)
install(FILES ${platform_files} DESTINATION ${FWB_INSTALL_DATADIR}/platform)
install(FILES ${help_files} DESTINATION ${FWB_INSTALL_DATADIR}/help/en_US)
install(DIRECTORY configlets DESTINATION ${FWB_INSTALL_DATADIR})
install(FILES fwbuilder.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES Icons/16x16/fwbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps)
install(FILES Icons/24x24/fwbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/24x24/apps)
install(FILES Icons/32x32/fwbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
install(FILES Icons/48x48/fwbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps)
install(FILES Icons/72x72/fwbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/72x72/apps)
install(FILES Icons/128x128/fwbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps)
install(FILES Icons/256x256/fwbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps)
install(FILES Icons/512x512/fwbuilder.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps)

