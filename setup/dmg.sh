#!/bin/sh

# This script automates dmg creation for GTester
# It requires the create-dmg command to exist on the path
# The create-dmg script is available from: https://github.com/andreyvit/yoursway-create-dmg

BUILD_DIR="$(pwd)"
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

test -f "${BUILD_DIR}/GTester.dmg" && rm -f "${BUILD_DIR}/GTester.dmg"
test -d "${BUILD_DIR}/dmg-temp" && rm -rf "${BUILD_DIR}/dmg-temp"
mkdir "${BUILD_DIR}/dmg-temp"

cp -a "${BUILD_DIR}/GTester.app" "${BUILD_DIR}/dmg-temp/"
macdeployqt "$BUILD_DIR/dmg-temp/GTester.app"

create-dmg \
	--volname GTester \
	--background "${SCRIPT_DIR}/../graphic/dmg-bg.png" \
	--window-size 500 400 \
	--icon-size 96 \
	--text-size 12 \
	--icon GTester.app 120 110 \
	--hide-extension GTester.app \
	--app-drop-link 380 110 \
	"${BUILD_DIR}/GTester.dmg" \
	"${BUILD_DIR}/dmg-temp"
