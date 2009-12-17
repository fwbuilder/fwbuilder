#-*- mode: makefile; tab-width: 4; -*-
#
#
include(../qmake.inc)

TEMPLATE = subdirs

CONFIG  += ordered

TARGET   = src
SUBDIRS  = res
# NOTE: 2.1 does not install scripts from src/tools ( fwb_install , fwb_compile_all)

contains( HAVE_ANTLR_RUNTIME, 1 ) {
  SUBDIRS += parsers
  contains ( HAVE_EXTERNAL_ANTLR, 0 ) {
    SUBDIRS += antlr
  }
}

SUBDIRS += common \
			compiler_lib \
			fwbedit \
			fwtransfer \
			iptlib \
			pflib \
			cisco_lib \
			gui \
			ipt \
			pf \
			ipf \
			ipfw \
			iosacl \
			pix \
			transfer_agents

