#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE= subdirs
CONFIG  += ordered

TARGET = unit_tests
SUBDIRS = dns_test inetaddrmask_test inet6addrmask_test

