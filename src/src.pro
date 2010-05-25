#-*- mode: makefile; tab-width: 4; -*-
#
#
include(../qmake.inc)

TEMPLATE = subdirs

CONFIG  += ordered

TARGET   = src

SUBDIRS  =  res \
			parsers \
			antlr \
			common \
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
			procurve_acl \
			transfer_agents

