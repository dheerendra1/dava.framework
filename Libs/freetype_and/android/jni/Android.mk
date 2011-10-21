#-----------------------------
# Freetype lib

# set local path for lib
LOCAL_PATH := $(call my-dir)

# clear all variables
include $(CLEAR_VARS)

# set module name
LOCAL_MODULE := libfreetype_android

# set path for includes
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include

# set exported includes
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES :=  \
                  ../../src/base/basepic.c \
                  ../../src/base/ftapi.c \
                  ../../src/base/ftbase.c \
                  ../../src/base/ftbbox.c \
                  ../../src/base/ftbitmap.c \
                  ../../src/base/ftdbgmem.c \
                  ../../src/base/ftdebug.c \
                  ../../src/base/ftglyph.c \
                  ../../src/base/ftinit.c \
                  ../../src/base/ftpic.c \
                  ../../src/base/ftstroke.c \
                  ../../src/base/ftsynth.c \
                  ../../src/base/ftsystem.c \
                  \
                  ../../src/cff/cffcmap.c \
                  ../../src/cff/cffdrivr.c \
                  ../../src/cff/cffgload.c \
                  ../../src/cff/cffload.c \
                  ../../src/cff/cffobjs.c \
                  ../../src/cff/cffparse.c \
                  ../../src/cff/cffpic.c \
                  \
                  ../../src/pshinter/pshalgo.c \
                  ../../src/pshinter/pshglob.c \
                  ../../src/pshinter/pshmod.c \
                  ../../src/pshinter/pshpic.c \
                  ../../src/pshinter/pshrec.c \
                  \
                  ../../src/psnames/psmodule.c \
                  ../../src/psnames/pspic.c \
                  \
                  ../../src/raster/ftraster.c \
                  ../../src/raster/ftrend1.c \
                  ../../src/raster/rastpic.c \
                  \
                  ../../src/sfnt/sfnt.c \
                  \
                  ../../src/smooth/smooth.c \
                  \
                  ../../src/truetype/truetype.c \

# set build flags
LOCAL_CFLAGS := -DFT2_BUILD_LIBRARY -g -O0

#set exported build flags
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)

# build static library
include $(BUILD_STATIC_LIBRARY)
