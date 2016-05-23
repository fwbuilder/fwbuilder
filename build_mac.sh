#!/bin/bash
export QMAKE_MAC_SDK="macosx"
export QMAKESPEC="macx-llvm"
export QMAKE_MACOSX_DEPLOYMENT_TARGET=10.6

./autogen.sh

echo "==> Compiling"
export JOBS=$(sysctl -n hw.ncpu | awk '{ print $1 + 1 }')
make -j${JOBS}

APP_VERSION=$(cat VERSION.h | grep VERSION | awk '{ print $3 }' | tr -d '"')
QT_VERSION_MAJOR=$(qmake -version | awk '/Using Qt version/ { ver=4; print $ver }' | awk -F. '{ major=1; print $major }')

FWBUILDER_ROOT_DIR=$(pwd)
TMP_BUNDLE_DIR=$(ls -d ${FWBUILDER_ROOT_DIR}/src/gui/*.app)
test -z "$TMP_BUNDLE_DIR" && {
    echo "Cannot find GUI bundle. My best guess was in ${FWBUILDER_ROOT_DIR}/src/gui/"
    exit 1
}

TMP_BUNDLE=$(basename $TMP_BUNDLE_DIR)
rm -rf $TMP_BUNDLE

echo "==> Copying base bundle"
cp -R ${FWBUILDER_ROOT_DIR}/src/gui/$TMP_BUNDLE .

echo "==> Copying resources and libraries into bundle"
mkdir -p ${TMP_BUNDLE}/Contents/Resources/os
mkdir -p ${TMP_BUNDLE}/Contents/Resources/platform
mkdir -p ${TMP_BUNDLE}/Contents/Resources/help
mkdir -p ${TMP_BUNDLE}/Contents/Resources/configlets
mkdir -p ${TMP_BUNDLE}/Contents/Resources/migration
cp -R ${FWBUILDER_ROOT_DIR}/src/res/{*.xml,os,platform,help,configlets} ${TMP_BUNDLE}/Contents/Resources/
cp ${FWBUILDER_ROOT_DIR}/src/libfwbuilder/etc/fwbuilder.dtd ${TMP_BUNDLE}/Contents/Resources/
cp ${FWBUILDER_ROOT_DIR}/src/libfwbuilder/migration/*.xslt ${TMP_BUNDLE}/Contents/Resources/migration/

echo "==> Copying binaries into bundle"
cp ${FWBUILDER_ROOT_DIR}/src/fwbedit/fwbedit.app/Contents/MacOS/fwbedit ${TMP_BUNDLE}/Contents/MacOS/
for binary in ipt pf ipf ipfw iosacl nxosacl pix procurve_acl
do
  cp ${FWBUILDER_ROOT_DIR}/src/$binary/fwb_$binary.app/Contents/MacOS/fwb_$binary ${TMP_BUNDLE}/Contents/MacOS/
done

BIN=${TMP_BUNDLE}/Contents/MacOS
chmod +x ${BIN}/fwb*

echo "==> Stripping binaries"
strip ${BIN}/*

echo "==> Running macdeployqt"
macdeployqt ${TMP_BUNDLE} -executable=${BIN}/fwbedit -executable=${BIN}/fwb_ipt -executable=${BIN}/fwb_pf -executable=${BIN}/fwb_ipf -executable=${BIN}/fwb_ipfw -executable=${BIN}/fwb_iosacl -executable=${BIN}/fwb_nxosacl -executable=${BIN}/fwb_pix -executable=${BIN}/fwb_procurve_acl 

echo "==> Done"
echo "# open ${TMP_BUNDLE}/"
