#!/bin/bash
# Creates files requred by the android build system
# Working dir must be the same as the script

source platform/android/paths.prop
clear

if [ -f $SDK_DIR/tools/android ]
  then
    echo "SDK_DIR: $SDK_DIR"
else
    tput bold
    echo "Error: SDK_DIR does not contain the path to Android SDK"
    echo "Open platform/android/paths.prop and setup correct paths to the SDK"
    tput sgr0
    exit
fi

if [ -f $NDK_DIR/build/core/ndk-common.sh ]
  then
    echo "NDK_DIR: $NDK_DIR"
else
    tput bold
    echo "Error: NDK_DIR does not contain the path to Android NDK"
    echo "Open platform/android/paths.prop and setup correct paths to the NDK"
    tput sgr0
    exit
fi

# updates sdk path in local.properties
echo
$SDK_DIR/tools/android update project --path `pwd`/platform/android

#create an application folder in NDK/apps/concept-engine
if [ ! -d $NDK_DIR/apps/concept-engine ]
  then
    mkdir $NDK_DIR/apps/concept-engine
fi
cp platform/android/Application.mk $NDK_DIR/apps/concept-engine/Application.mk

ESC_PATH=$(echo `pwd`|sed -e 's/\(\/\|\\\|&\)/\\&/g')
perl -pi -e 's/REPLACE_ME/'$ESC_PATH'/g' $NDK_DIR/apps/concept-engine/Application.mk

