#-*- mode: makefile; tab-width: 4; -*-
#

include(../qmake.inc)

TEMPLATE = lib

TARGET = migration

win32 {
	CONFIG -= embed_manifest_exe
}

QMAKE_RUN_CC  = "@echo"
QMAKE_RUN_CXX = "@echo"
QMAKE_LINK = "@echo"

target.path = $${migration.path}

target.files  = FWObjectDatabase_0.9.0.xslt \
				FWObjectDatabase_0.9.1.xslt \
				FWObjectDatabase_0.9.2.xslt \
				FWObjectDatabase_0.9.3.xslt \
				FWObjectDatabase_0.9.4.xslt \
				FWObjectDatabase_0.9.5.xslt \
				FWObjectDatabase_0.10.0.xslt \
				FWObjectDatabase_0.10.1.xslt \
				FWObjectDatabase_0.10.2.xslt \
				FWObjectDatabase_0.10.3.xslt \
				FWObjectDatabase_0.10.4.xslt \
				FWObjectDatabase_0.10.5.xslt \
				FWObjectDatabase_0.10.6.xslt \
				FWObjectDatabase_0.10.7.xslt \
				FWObjectDatabase_0.10.8.xslt \
				FWObjectDatabase_0.10.9.xslt \
				FWObjectDatabase_0.10.10.xslt \
				FWObjectDatabase_0.10.11.xslt \
				FWObjectDatabase_0.10.12.xslt \
				FWObjectDatabase_0.10.13.xslt \
				FWObjectDatabase_0.10.14.xslt \
				FWObjectDatabase_1.0.0.xslt \
				FWObjectDatabase_1.0.1.xslt \
				FWObjectDatabase_1.0.2.xslt \
				FWObjectDatabase_2.0.0.xslt \
				FWObjectDatabase_2.0.1.xslt \
				FWObjectDatabase_2.0.2.xslt \
				FWObjectDatabase_2.0.3.xslt \
				FWObjectDatabase_2.0.4.xslt \
				FWObjectDatabase_2.0.5.xslt \
				FWObjectDatabase_2.0.6.xslt \
				FWObjectDatabase_2.0.7.xslt \
				FWObjectDatabase_2.0.8.xslt \
				FWObjectDatabase_2.0.9.xslt \
				FWObjectDatabase_2.0.10.xslt \
				FWObjectDatabase_2.0.11.xslt \
				FWObjectDatabase_2.0.12.xslt \
				FWObjectDatabase_2.0.99.xslt \
				FWObjectDatabase_2.1.0.xslt \
				FWObjectDatabase_2.1.1.xslt \
				FWObjectDatabase_2.1.2.xslt \
				FWObjectDatabase_2.1.3.xslt \
                FWObjectDatabase_2.1.4.xslt \
                FWObjectDatabase_2.1.5.xslt \
                FWObjectDatabase_2.1.6.xslt \
                FWObjectDatabase_2.1.7.xslt \
                FWObjectDatabase_2.1.8.xslt \
                FWObjectDatabase_2.1.9.xslt \
                FWObjectDatabase_2.1.10.xslt \
                FWObjectDatabase_2.1.11.xslt \
                FWObjectDatabase_2.1.12.xslt \
                FWObjectDatabase_2.1.13.xslt \
                FWObjectDatabase_2.1.14.xslt \
                FWObjectDatabase_2.1.15.xslt \
                FWObjectDatabase_2.1.16.xslt \
                FWObjectDatabase_2.1.17.xslt \
                FWObjectDatabase_2.1.18.xslt \
                FWObjectDatabase_2.1.19.xslt \
                FWObjectDatabase_2.1.99.xslt \
                FWObjectDatabase_3.xslt  \
                FWObjectDatabase_4.xslt  \
                FWObjectDatabase_5.xslt  \
                FWObjectDatabase_6.xslt  \
                FWObjectDatabase_7.xslt  \
                FWObjectDatabase_8.xslt  \
                FWObjectDatabase_9.xslt  \
                FWObjectDatabase_10.xslt \
                FWObjectDatabase_11.xslt \
                FWObjectDatabase_12.xslt \
                FWObjectDatabase_13.xslt \
                FWObjectDatabase_14.xslt \
                FWObjectDatabase_15.xslt \
                FWObjectDatabase_16.xslt \
                FWObjectDatabase_17.xslt \
                FWObjectDatabase_18.xslt \
                FWObjectDatabase_19.xslt \
                FWObjectDatabase_20.xslt \
                FWObjectDatabase_21.xslt \


