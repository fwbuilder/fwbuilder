include(../tests_common.pri)

QT += testlib network gui
TARGET = DiscoveryDruidTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main_DiscoveryDruidTest.cpp \
    DiscoveryDruidTest.cpp
    
HEADERS += DiscoveryDruidTest.h
