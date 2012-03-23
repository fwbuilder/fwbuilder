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

MODULE_NAME=$1
BUNDLE=$2

test -z "$BUNDLE" && {
    echo "Usage: fwbuilder-makedmg.sh MODULE_NAME <BUNDLE_NAME>"
    exit 1
}

# assuming the top of fwbuilder module source code is cwd when this
# script is executed
BUILD_ROOT_DIR="../../"

. VERSION

BASE_DMG_FILE_NAME=$(basename $BUNDLE | sed 's/\.app/.dmg/')
BASE_RO_DMG_FILE_NAME=$(basename $BUNDLE | sed 's/\.app/-ro.dmg/')

SLA_FILE="packaging/SLA.r"
DMG_FILE="${BUILD_ROOT_DIR}/$BASE_DMG_FILE_NAME"
DMG_RO_FILE="${BUILD_ROOT_DIR}/${BASE_RO_DMG_FILE_NAME}"

/usr/bin/hdiutil create -srcfolder $BUNDLE \
    -ov -fs HFS+ -volname "Firewall Builder $VERSION" $DMG_FILE
# chmod 0664 $DMG_FILE

# ################################################################
# How to add license agreement to the dms image
#
# http://developer.apple.com/documentation/DeveloperTools/Conceptual/SoftwareDistribution/Containers/chapter_3_section_4.html
#
# Need "Software License Agreements for UDIFs (DMG)"
# http://developer.apple.com/sdk/index.html
# Instructions are in this package. The SLA prototype in this SDK was in
# a format that requires resource editor which I could not find. However
# SLA prototype file provided in the buildDMG package found at
# http://www.objectpark.org/buildDMG.html was usable. The prototype comes
# in file SLA.r which can be edited with XCode (just do "open SLA.r").
# I added NetCitadel license text to the file SLA.r and checked it in
# as modules/fwbuilder/files/packaging/SLA.r

hdiutil convert -format UDCO $DMG_FILE -o $DMG_RO_FILE
hdiutil unflatten $DMG_RO_FILE
Rez /Developer/Headers/FlatCarbon/*.r  $SLA_FILE -a -o $DMG_RO_FILE
hdiutil flatten $DMG_RO_FILE
mv $DMG_RO_FILE $DMG_FILE

