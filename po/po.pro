#-*- mode: makefile; tab-width: 4; -*-
#

include(../qmake.inc)

win32 {
  QMAKE_RUN_CC  = @echo
  QMAKE_RUN_CXX = @echo
  QMAKE_LINK    = @echo
}
!win32 {
  QMAKE_RUN_CC  = @echo > /dev/null
  QMAKE_RUN_CXX = @echo > /dev/null
  QMAKE_LINK    = @echo > /dev/null
}

TARGET        = po

!win32:LOCALEINSTALLDIR = $(INSTALL_ROOT)/$$PKGLOCALEDIR
win32:LOCALEINSTALLDIR  = $$PKGLOCALEDIR

de.path  = $$PKGLOCALEDIR
!win32:de.extra = /usr/bin/install -c -m 644 de.qm $$LOCALEINSTALLDIR/fwbuilder_de.qm
win32:de.extra = -$(INSTALL_FILE) de.qm $$LOCALEINSTALLDIR\fwbuilder_de.qm

es.path  = $$PKGLOCALEDIR
!win32:es.extra = /usr/bin/install -c -m 644 es.qm $$LOCALEINSTALLDIR/fwbuilder_es.qm
win32:es.extra = -$(INSTALL_FILE) es.qm $$LOCALEINSTALLDIR\fwbuilder_es.qm

fr.path  = $$PKGLOCALEDIR
!win32:fr.extra = /usr/bin/install -c -m 644 fr.qm $$LOCALEINSTALLDIR/fwbuilder_fr.qm
win32:fr.extra = -$(INSTALL_FILE) fr.qm $$LOCALEINSTALLDIR\fwbuilder_fr.qm

ja.path  = $$PKGLOCALEDIR
!win32:ja.extra = /usr/bin/install -c -m 644 ja.qm $$LOCALEINSTALLDIR/fwbuilder_ja.qm
win32:ja.extra = -$(INSTALL_FILE) ja.qm $$LOCALEINSTALLDIR\fwbuilder_ja.qm

pt_BR.path  = $$PKGLOCALEDIR
!win32:pt_BR.extra = /usr/bin/install -c -m 644 pt_BR.qm $$LOCALEINSTALLDIR/fwbuilder_pt_BR.qm
win32:pt_BR.extra = -$(INSTALL_FILE) pt_BR.qm $$LOCALEINSTALLDIR\fwbuilder_pt_BR.qm

ru.path  = $$PKGLOCALEDIR
!win32:ru.extra = /usr/bin/install -c -m 644 ru.qm $$LOCALEINSTALLDIR/fwbuilder_ru.qm
win32:ru.extra = -$(INSTALL_FILE) ru.qm $$LOCALEINSTALLDIR\fwbuilder_ru.qm

sv.path  = $$PKGLOCALEDIR
!win32:sv.extra = /usr/bin/install -c -m 644 sv.qm $$LOCALEINSTALLDIR/fwbuilder_sv.qm
win32:sv.extra = -$(INSTALL_FILE) sv.qm $$LOCALEINSTALLDIR\fwbuilder_sv.qm

INSTALLS   -= target
INSTALLS   += de es fr ja pt_BR ru sv

