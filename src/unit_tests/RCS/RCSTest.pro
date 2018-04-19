include(../tests_common.pri)

QT += gui widgets printsupport network

HEADERS	+= RCSTest.h
SOURCES	+=  main_RCS.cpp \
        RCSTest.cpp
TARGET = RCSTest
