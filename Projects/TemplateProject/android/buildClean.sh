#!/bin/sh

echo "*****************   START   ****************"

echo "\$0=$0"
cd `dirname $0`

echo " "

echo "PWD=`pwd`"

echo "Remove Data"
rm -f -r -v `pwd`/assets/Data
echo "Remove Done"


SDK_ROOT=`pwd`/../../dava.framework/Sources

export NDK_MODULE_PATH=`pwd`/jni:$SDK_ROOT/External:$SDK_ROOT/External/Box2D:$SDK_ROOT/Internal:$SDK_ROOT


ndk-build clean
if [ $? != 0 ]; then
    echo "ERROR: Can't build test program!"
    exit 1
fi

echo "*****************   FINISH   ****************"
