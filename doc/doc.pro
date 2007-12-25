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
              README.routing          \
			  README.iosacl           \
			  README.policy_import    \
			  FWBuilder-Routing-LICENSE.txt \
              PatchAcceptancePolicy.txt \
			  ReleaseNotes_2.1.7.html   \
			  ReleaseNotes_2.1.7.txt    \
			  ReleaseNotes_2.1.8.html   \
			  ReleaseNotes_2.1.8.txt    \
			  ReleaseNotes_2.1.9.html   \
			  ReleaseNotes_2.1.9.txt    \
			  ReleaseNotes_2.1.10.html  \
			  ReleaseNotes_2.1.10.txt   \
			  ReleaseNotes_2.1.11.html  \
			  ReleaseNotes_2.1.11.txt   \
			  ReleaseNotes_2.1.12.html  \
			  ReleaseNotes_2.1.12.txt   \
			  ReleaseNotes_2.1.13.html  \
			  ReleaseNotes_2.1.13.txt   \
			  ReleaseNotes_2.1.14.html  \
			  ReleaseNotes_2.1.14.txt   \
			  ReleaseNotes_2.1.15.html  \
			  ReleaseNotes_2.1.15.txt

doc.path    = $$DOCDIR

man.files   = fwbedit.1             \
              fwblookup.1           \
              fwbuilder.1           \
              fwb_ipf.1             \
              fwb_ipfw.1            \
              fwb_ipt.1             \
              fwb_pf.1              \

#             fwb_install.1           \
#			  fwb_compile_all.1       \

man.path    = $$MANDIR/man1

INSTALLS   -= target
INSTALLS   += doc
INSTALLS   += man
