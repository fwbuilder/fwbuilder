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
			libgui \
			fwbedit \
			gui \

