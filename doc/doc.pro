#-*- mode: makefile; tab-width: 4; -*-
#

include(../qmake.inc)

TEMPLATE = aux

win32 {
	CONFIG -= embed_manifest_exe
}

win32 {
  QMAKE_RUN_CC  = echo
  QMAKE_RUN_CXX = echo
  QMAKE_LINK    = echo
}

!win32 {
  QMAKE_RUN_CC  = @echo > /dev/null
  QMAKE_RUN_CXX = @echo > /dev/null
  QMAKE_LINK    = @echo > /dev/null
}

TARGET      = doc

doc.files   = AUTHORS                 \
              ChangeLog               \
              COPYING                 \
              Credits                 \
              README.floppyfw         \
              README.ipf              \
              README.ipfw             \
              README.ipt              \
              README.pf               \
              README.pix              \
              README.pix_routing      \
              README.routing          \
			  README.iosacl           \
			  README.policy_import    \
			  FWBuilder-Routing-LICENSE.txt \
              PatchAcceptancePolicy.txt

doc.path    = $$DOCDIR

man.files   = fwbedit.1             \
              fwbuilder.1           \
              fwb_iosacl.1          \
              fwb_ipf.1             \
              fwb_ipfw.1            \
              fwb_ipt.1             \
              fwb_pf.1              \
              fwb_pix.1             \
              export_secuwall.1     \

#             fwb_install.1           \
#			  fwb_compile_all.1       \

man.path    = $$MANDIR/man1

INSTALLS   -= target
INSTALLS   += doc
INSTALLS   += man
