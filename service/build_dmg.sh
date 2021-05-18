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
pipenv run pyinstaller --name CheckMeet --osx-bundle-identifier com.formlabs.hackathon2021.checkmeet --windowed "${SRC}/launch_service.py"
codesign -s CheckMeet --deep dist/CheckMeet.app
hdiutil create -volname CheckMeet -srcfolder dist/CheckMeet.app -ov -format UDZO "${SRC}/CheckMeet.dmg"
