#-*- mode: makefile; tab-width: 4; -*-
#

include(../../qmake.inc)

TEMPLATE = app

win32 {
  CONFIG -= embed_manifest_exe
  CONFIG -= debug
  QMAKE_RUN_CC  = echo
  QMAKE_RUN_CXX = echo
  QMAKE_LINK    = echo
}

!win32 {
  QMAKE_RUN_CC  = @echo > /dev/null
  QMAKE_RUN_CXX = @echo > /dev/null
  QMAKE_LINK    = @echo > /dev/null
}

TARGET = res

win32:target.path = $$PREFIX/
unix:target.path = $$PREFIX/share/fwbuilder/
macx:target.path = $$PREFIX/

res.files = objects_init.xml templates.xml resources.xml
res_os.files = os/*.xml
res_platform.files = platform/*.xml
res_help_en_US.files = help/en_US/*.html help/en_US/*.png help/en_US/*.jpg
res_configlets.files = configlets/*


INSTALLS -= target
INSTALLS += res 
INSTALLS += res_os
INSTALLS += res_platform
INSTALLS += res_help_en_US
INSTALLS += res_configlets

unix {
!macx {
	res_desktop.files = fwbuilder.desktop
	INSTALLS += res_desktop

	!isEmpty(ICONSDIR) {
		app_icon_16x16.files = Icons/16x16/fwbuilder.png
		app_icon_16x16.path = $$ICONSDIR/16x16/apps
		INSTALLS += app_icon_16x16

		app_icon_24x24.files = Icons/24x24/fwbuilder.png
		app_icon_24x24.path = $$ICONSDIR/24x24/apps
		INSTALLS += app_icon_24x24

		app_icon_32x32.files = Icons/32x32/fwbuilder.png
		app_icon_32x32.path = $$ICONSDIR/32x32/apps
		INSTALLS += app_icon_32x32

		app_icon_48x48.files = Icons/48x48/fwbuilder.png
		app_icon_48x48.path = $$ICONSDIR/48x48/apps
		INSTALLS += app_icon_48x48

		app_icon_72x72.files = Icons/72x72/fwbuilder.png
		app_icon_72x72.path = $$ICONSDIR/72x72/apps
		INSTALLS += app_icon_72x72

		app_icon_128x128.files = Icons/128x128/fwbuilder.png
		app_icon_128x128.path = $$ICONSDIR/128x128/apps
		INSTALLS += app_icon_128x128

		app_icon_256x256.files = Icons/256x256/fwbuilder.png
		app_icon_256x256.path = $$ICONSDIR/256x256/apps
		INSTALLS += app_icon_256x256

		app_icon_512x512.files = Icons/512x512/fwbuilder.png
		app_icon_512x512.path = $$ICONSDIR/512x512/apps
		INSTALLS += app_icon_512x512
	}
}
}



