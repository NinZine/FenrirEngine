#!/bin/bash
# Build script for android:
#	./android.sh
#		compiles ndk and sdk code
#	./android.sh install
#		compile and transfer to running emulator
#	./android.sh uninstall
#		uninstall package from running emulator
#
# '> /dev/null' is to hide output of pushd and popd

source platform/android/paths.prop
clear

# Uninstall then exit to avoid compile
if [ "$1" == "uninstall" ]
  then
    pushd ./platform/android > /dev/null
    ant $1
    popd > /dev/null
    exit
fi

# Check if NDK_DIR is valid
if [ ! -f $NDK_DIR/build/core/ndk-common.sh ]
  then
    echo "Error: correct path variables in platform/android/paths.prop and run android_build.sh"
    exit
fi

# creates local.properties
echo
$SDK_DIR/tools/android update project --path `dirname $0`/platform/android

echo
#create an application folder in NDK/apps/concept-engine
if [ ! -d obj/android ]
  then
    mkdir obj/android
fi
cp platform/android/default.properties obj/android/default.properties

# compiles NDK code
echo
pushd $NDK_DIR > /dev/null
make APP=concept-engine
popd > /dev/null

# compiles SDK code
echo
echo "Beginning compile of SDK code..."
pushd ./platform/android > /dev/null
ant $1
popd > /dev/null
