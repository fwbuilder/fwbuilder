TEMPLATE	= app
LANGUAGE	= C++

HEADERS	+= ../../../config.h \
	../RCS.h \
        ../utils.h \
	../global.h

SOURCES	+=  unit_tests.cpp \
        ../utils.cpp \
        ../RCS.cpp


#
include(../../../qmake.inc)
#
exists(../qmake.inc) {
  include( ../qmake.inc)
}


INCLUDEPATH   += .. 
INCLUDEPATH   += ../../../

#
#
#
#
TARGET = unit_tests
#

#unix  {
#  !macx {
#      #  }
#}

QMAKE_COPY    = ../../install.sh -m 0755 -s
