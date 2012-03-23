#!/bin/sh
#
#                           Firewall Builder
#
#                  Copyright (C) 2012 NetCitadel, LLC
#
#   Author:  Vadim Kurland     vadim@netcitadel.com
#
#   This program is free software which we release under the GNU General Public
#   License. You may redistribute and/or modify this program under the terms
#   of that license as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   To get a copy of the GNU General Public License, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#   This script assembles Mac OS X bundle
#
#---------------------------------------------------------------------
#
#


function usage {
  echo "makebundle MODULE_NAME BUNDLE_ROOT"
  exit 1
}

unset DISPLAY
unset LD_LIBRARY_PATH

MODULE_NAME=$1
BUNDLE_ROOT=$2

test -z "$BUNDLE_ROOT" && {
  usage
  exit 1
}

export BUNDLE_ROOT

# Assuming script is being run with fwbuilder top directory as
# cwd. Need to switch to the top level build environment dir.

test -d 'tools' || cd ../..

pwd

eval $(python tools/module.py -s $MODULE_NAME)

BUILD_ROOT_DIR=$(pwd)

test -d ${BUILD_ROOT_DIR}/source || {
    echo "Can't find directory where source tree should be located: ${BUILD_ROOT_DIR}/source"
    exit 1
}

#VERSIONED_MODULE_NAME=$(basename $BUNDLE_ROOT | sed 's/\.app//')
VERSIONED_MODULE_NAME="$MODULE_NAME-$VERSION"

echo "Module name: $MODULE_NAME"
echo "Versioned module name: $VERSIONED_MODULE_NAME"

QT_FRAMEWORKS="QtCore QtGui QtNetwork QtDBus QtXml"

FWBUILDER_ROOT_DIR=$(find ${BUILD_ROOT_DIR}/source -name "$VERSIONED_MODULE_NAME" -o -name $MODULE_NAME -depth 1)

echo
echo "=== Building bundle ${BUNDLE_ROOT} "
echo 

TMP_BUNDLE_DIR=$(ls -d ${FWBUILDER_ROOT_DIR}/src/gui/*.app)
test -z "$TMP_BUNDLE_DIR" && {
    echo "Can not find GUI bundle inside of ${FWBUILDER_ROOT_DIR}/src/gui"
    exit 1
}
TMP_BUNDLE=$(basename $TMP_BUNDLE_DIR)
rm -rf $TMP_BUNDLE

cp -R ${FWBUILDER_ROOT_DIR}/src/gui/$TMP_BUNDLE .

echo "=== Running macdeployqt"
macdeployqt ${TMP_BUNDLE}


echo "=== Copying fwbuilder resources and libfwbuilder libraries"
mkdir -p ${TMP_BUNDLE}/Contents/Resources
mkdir -p ${TMP_BUNDLE}/Contents/Resources/os
mkdir -p ${TMP_BUNDLE}/Contents/Resources/platform
mkdir -p ${TMP_BUNDLE}/Contents/Resources/help
mkdir -p ${TMP_BUNDLE}/Contents/Resources/configlets

(cd ${FWBUILDER_ROOT_DIR}/src/res/; tar cf - --exclude .svn {*.xml,os/*.xml,platform/*.xml,help,configlets} ) | \
  tar -C  ${TMP_BUNDLE}/Contents/Resources -xf -

mkdir -p ${TMP_BUNDLE}/Contents/Frameworks

#echo "=== Copying libfwbuilder"
#
#cp ${LIBFWBUILDER_ROOT_DIR}/src/fwbuilder/libfwbuilder.?.dylib \
#    ${TMP_BUNDLE}/Contents/Frameworks/
#cp ${LIBFWBUILDER_ROOT_DIR}/src/fwcompiler/libfwcompiler.?.dylib \
#    ${TMP_BUNDLE}/Contents/Frameworks/
#
#chmod u+w ${TMP_BUNDLE}/Contents/Frameworks/*

cp ${FWBUILDER_ROOT_DIR}/src/libfwbuilder/etc/fwbuilder.dtd  ${TMP_BUNDLE}/Contents/Resources

mkdir -p ${TMP_BUNDLE}/Contents/Resources/migration
cp ${FWBUILDER_ROOT_DIR}/src/libfwbuilder/migration/*.xslt ${TMP_BUNDLE}/Contents/Resources/migration



echo "=== Copying binaries"
cp -r ${FWBUILDER_ROOT_DIR}/src/fwbedit/fwbedit.app/Contents/MacOS/fwbedit \
    ${TMP_BUNDLE}/Contents/MacOS/

for c in ipt pf ipf ipfw iosacl pix procurve_acl
do
  cp -r ${FWBUILDER_ROOT_DIR}/src/$c/fwb_$c.app/Contents/MacOS/fwb_$c \
    ${TMP_BUNDLE}/Contents/MacOS/
done

chmod +x ${TMP_BUNDLE}/Contents/MacOS/fwb*

for f in ${TMP_BUNDLE}/Contents/MacOS/fwb*
do
  for framework in $QT_FRAMEWORKS
  do
    install_name_tool -change ${framework}.framework/Versions/4/${framework} \
      @executable_path/../Frameworks/${framework}.framework/Versions/4/${framework} \
      $f
  done
done

rm -rf $BUNDLE_ROOT
mv $TMP_BUNDLE $BUNDLE_ROOT


exit 0



echo "=== Copying qt.conf file"
cp ${FWBUILDER_ROOT_DIR}/packaging/qt.conf ${BUNDLE_ROOT}/Contents/Resources 

echo "=== Copying .nib files"
test -d /Library/Frameworks/QtGui.framework/Versions/4/Resources && \
    cp -R /Library/Frameworks/QtGui.framework/Versions/4/Resources/* ${BUNDLE_ROOT}/Contents/Resources 

# we have no working translations in v4 and v5
# echo "=== Copying locales"
# mkdir -p ${BUNDLE_ROOT}/Contents/Resources/locale
# cp -r ${FWBUILDER_ROOT_DIR}/src/gui/*.qm ${BUNDLE_ROOT}/Contents/Resources/locale

qt_translations=$(find /Developer/Applications/Qt/translations/ -name '*.qm')
test -n "$qt_translations" && {
  for f in $qt_translations; do
    cp $f ${BUNDLE_ROOT}/Contents/Resources/locale/
  done
}


echo "=== Setting up copies of QT frameworks"

for framework in $QT_FRAMEWORKS
do
    test -d /Library/Frameworks/${framework}.framework/Versions && {
        cp -R /Library/Frameworks/${framework}.framework/Versions/4/${framework} ${BUNDLE_ROOT}/Contents/Frameworks/
        install_name_tool -id @executable_path/../Frameworks/${framework} \
            ${BUNDLE_ROOT}/Contents/Frameworks/${framework}
    }
done

mkdir -p ${BUNDLE_ROOT}/Contents/plugins
cp /Developer//Applications/Qt/plugins/accessible/libqtaccessiblewidgets.dylib \
  ${BUNDLE_ROOT}/Contents/plugins/


install_name_tool -change QtCore.framework/Versions/4/QtCore \
    @executable_path/../Frameworks/QtCore \
    ${BUNDLE_ROOT}/Contents/Frameworks/QtGui

install_name_tool -change QtCore.framework/Versions/4/QtCore \
    @executable_path/../Frameworks/QtCore \
    ${BUNDLE_ROOT}/Contents/Frameworks/QtNetwork

install_name_tool -change QtCore.framework/Versions/4/QtCore \
    @executable_path/../Frameworks/QtCore \
    ${BUNDLE_ROOT}/Contents/Frameworks/QtXml

test -f ${BUNDLE_ROOT}/Contents/Frameworks/QtDBus && {
    install_name_tool -change QtCore.framework/Versions/4/QtCore \
        @executable_path/../Frameworks/QtCore \
        ${BUNDLE_ROOT}/Contents/Frameworks/QtDBus
    install_name_tool -change QtXml.framework/Versions/4/QtXml \
        @executable_path/../Frameworks/QtXml \
        ${BUNDLE_ROOT}/Contents/Frameworks/QtDBus
}

install_name_tool -change QtCore.framework/Versions/4/QtCore \
    @executable_path/../Frameworks/QtCore \
    ${BUNDLE_ROOT}/Contents/plugins/libqtaccessiblewidgets.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui \
    @executable_path/../Frameworks/QtGui \
    ${BUNDLE_ROOT}/Contents/plugins/libqtaccessiblewidgets.dylib


for f in ${BUNDLE_ROOT}/Contents/MacOS/fwb*
do
  for framework in $QT_FRAMEWORKS
  do
    install_name_tool -change ${framework}.framework/Versions/4/${framework} \
      @executable_path/../Frameworks/${framework} \
      $f
  done
done



