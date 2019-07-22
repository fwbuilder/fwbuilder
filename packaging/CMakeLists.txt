set(CPACK_PACKAGE_VENDOR fwbuilder)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY fwbuilder)
set(CPACK_PACKAGE_EXECUTABLES fwbuilder fwbuilder)

if(APPLE)
  set(BUNDLE_DIR "\${CMAKE_INSTALL_PREFIX}/fwbuilder.app")
  set(BUNDLE_BIN_DIR "${BUNDLE_DIR}/Contents/MacOS")
  set(BUNDLE_FRAMEWORKS_DIR "${BUNDLE_DIR}/Contents/Frameworks")
  set(BUNDLE_PLUGINS_DIR "${BUNDLE_DIR}/Contents/PlugIns")
  set(MACDEPLOYQT_BIN "${_qt5Core_install_prefix}/bin/macdeployqt")

  install(CODE "
    EXECUTE_PROCESS(COMMAND ${MACDEPLOYQT_BIN} ${BUNDLE_DIR}
    -executable=${BUNDLE_BIN_DIR}/fwbedit
    -executable=${BUNDLE_BIN_DIR}/fwb_iosacl
    -executable=${BUNDLE_BIN_DIR}/fwb_ipt
    -executable=${BUNDLE_BIN_DIR}/fwb_ipf
    -executable=${BUNDLE_BIN_DIR}/fwb_ipfw
    -executable=${BUNDLE_BIN_DIR}/fwb_junosacl
    -executable=${BUNDLE_BIN_DIR}/fwb_nxosacl
    -executable=${BUNDLE_BIN_DIR}/fwb_procurve_acl
    -executable=${BUNDLE_BIN_DIR}/fwb_pf
    WORKING_DIRECTORY \"\${CMAKE_BINARY_DIR}\"
    OUTPUT_FILE macdeployqt-fwbuilder.log
    ERROR_FILE macdeployqt-fwbuilder.error)
    ")

  install(CODE "
    EXECUTE_PROCESS(COMMAND rm -rf
      ${BUNDLE_FRAMEWORKS_DIR}/QtQml.framework
      ${BUNDLE_FRAMEWORKS_DIR}/QtQuick.framework
      ${BUNDLE_FRAMEWORKS_DIR}/QtSvg.framework
      ${BUNDLE_FRAMEWORKS_DIR}/QtVirtualKeyboard.framework
      ${BUNDLE_PLUGINS_DIR}/virtualkeyboard
      ${BUNDLE_PLUGINS_DIR}/imageformats
    )
    ")

  set(CPACK_GENERATOR "DragNDrop")
  set(CPACK_DMG_FORMAT "UDBZ")
  set(CPACK_DMG_VOLUME_NAME fwbuilder)
  set(CPACK_PACKAGE_FILE_NAME fwbuilder-${PROJECT_VERSION})
elseif(WIN32)
configure_file(fwbuilder.nsi.in fwbuilder.nsi @ONLY)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/fwbuilder.nsi
    fwbuilder-160x60.bmp
    fwbuilder-164x314.bmp
  DESTINATION ${CMAKE_INSTALL_PREFIX}/release)
endif()

include(CPack)
