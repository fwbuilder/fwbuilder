#-*- mode: makefile; tab-width: 4; -*-
#

include(../qmake.inc)

TEMPLATE = lib

CONFIG -= embed_manifest_exe

QMAKE_RUN_CC  = echo
QMAKE_RUN_CXX = echo
QMAKE_LINK    = echo

TARGET = 

target.files  = AUTHORS                 \
                ChangeLog               \
                COPYING                 \
                Credits                 \
			    INSTALL                 \
                PatchAcceptancePolicy.txt

target.path   = $$doc.path

#INSTALLS     -= target
#INSTALLS     += doc
