#-*- mode: makefile; tab-width: 4; -*-
#
#
include(../qmake.inc)

TEMPLATE = subdirs

CONFIG  += ordered debug

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
			nxosacl \
			iosacl \
			pix \
			procurve_acl \
            juniper_lib \
            junosacl \
			libgui \
			fwbedit \
			gui \

