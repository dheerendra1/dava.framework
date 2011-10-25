#!/bin/sh

echo "*****************   START   ****************"

echo "\$0=$0"
cd `dirname $0`

echo " "

echo "PWD=`pwd`"

export NDK_MODULE_PATH=`pwd`/jni

ndk-build NDK_DEBUG=0
if [ $? != 0 ]; then
    echo "ERROR: Can't build test program!"
    exit 1
fi

cp $NDK_MODULE_PATH/../obj/local/armeabi/libpng_android.a $NDK_MODULE_PATH/../../../libs/

echo "*****************   FINISH   ****************"
