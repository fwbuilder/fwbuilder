#-*- mode: makefile; tab-width: 4; -*-
#
#
include(../qmake.inc)

TEMPLATE = subdirs

CONFIG  += ordered

TARGET   = src

SUBDIRS  =  libfwbuilder \
			res \
			antlr \
			parsers \
            import \
			common \
			compiler_lib \
			fwbedit \
			fwtransfer \
			iptlib \
			pflib \
			cisco_lib \
			libgui \
			gui \
			ipt \
			pf \
			ipf \
			ipfw \
			iosacl \
			pix \
			procurve_acl \
			transfer_agents

