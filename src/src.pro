#-*- mode: makefile; tab-width: 4; -*-
#
#
include(../qmake.inc)

TEMPLATE = subdirs

CONFIG  += ordered

TARGET   = src
SUBDIRS  = res fwbedit fwblookup
# NOTE: 2.1 does not install scripts from src/tools ( fwb_install , fwb_compile_all)

contains( HAVE_ANTLR_RUNTIME, 1 ) {
  SUBDIRS += parsers
  contains ( HAVE_EXTERNAL_ANTLR, 0 ) {
    SUBDIRS += antlr
  }
}

SUBDIRS += gui ipt pflib pf ipf ipfw

