#!/bin/sh

echo "*****************   START   ****************"

echo "\$0=$0"
cd `dirname $0`

echo " "

echo "PWD=`pwd`"

SDK_ROOT="/Users/klesch/Work/android/Sources"


#echo "SDK_ROOT=$SDK_ROOT"

export NDK_MODULE_PATH=`pwd`/jni:$SDK_ROOT/External:$SDK_ROOT/External/Yaml:$SDK_ROOT/External/Box2D:$SDK_ROOT/External/Freetype:$SDK_ROOT/Internal:$SDK_ROOT


echo ""
#echo "NDK_MODULE_PATH=$NDK_MODULE_PATH"
echo ""

ndk-build NDK_DEBUG=0
if [ $? != 0 ]; then
    echo "ERROR: Can't build test program!"
    exit 1
fi

echo "*****************   FINISH   ****************"
