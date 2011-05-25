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
			fwtransfer \
			iptlib \
			ipt \
			pflib \
			pf \
			ipf \
			ipfw \
			cisco_lib \
			iosacl \
			pix \
			procurve_acl \
			transfer_agents \
			libgui \
			fwbedit \
			gui \

