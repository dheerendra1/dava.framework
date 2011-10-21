#!/bin/sh

echo "*****************   START   ****************"

echo "\$0=$0"
cd `dirname $0`

echo " "

echo "PWD=`pwd`"

#SDK_ROOT="/Users/klesch/Work/WoT/Framework/dava.framework/Sources"
SDK_ROOT=`pwd`/../../../Sources

echo ""
echo "SDK_ROOT = $SDK_ROOT"
echo ""


export NDK_MODULE_PATH=`pwd`/jni:$SDK_ROOT/External:$SDK_ROOT/External/Box2D:$SDK_ROOT/Internal:$SDK_ROOT



ndk-build NDK_DEBUG=1
if [ $? != 0 ]; then
    echo "ERROR: Can't build test program!"
    exit 1
fi

echo "*****************   FINISH   ****************"
