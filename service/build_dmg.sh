#!/bin/sh -uex

# IMPORTANT: create a certificate named CheckMeet like this:
#   keychain access -> menu -> certificate assistant -> create a certificate -> name: "CheckMeet"

# This script is best to be run from pipenv:
#   $ pipenv --rm
#   $ pipenv install --skip-lock
#   $ pipenv run ./create_dmg.sh

SCRATCH=$(mktemp -d -t build_dmg.XXXXXXXX)
cleanup() {
	rm -rf "${SCRATCH}"
}
trap cleanup EXIT

SRC=$(pwd)
cd "$SCRATCH"

pip install https://github.com/pyinstaller/pyinstaller/archive/develop.zip
# pyinstaller --onefile doesn't work because https://github.com/pyinstaller/pyinstaller/pull/3991 hasn't been merged yet
pyinstaller --clean --name CheckMeet --osx-bundle-identifier com.formlabs.hackathon2021.checkmeet --windowed "${SRC}/launch_service.py"
codesign --force --verbose --sign CheckMeet --deep dist/CheckMeet.app
# pip install dmgbuild is a more customizable alternative of hdiutil
hdiutil create -volname CheckMeet -srcfolder dist/CheckMeet.app -ov -format UDZO "${SRC}/CheckMeet.dmg"
