#-----------------------------
# Yaml lib

# set local path for lib
LOCAL_PATH := $(call my-dir)

# clear all variables
include $(CLEAR_VARS)

# set module name
LOCAL_MODULE    := libyaml_android

# set path for includes
LOCAL_C_INCLUDES := $(LOCAL_PATH)

# set exported includes
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src

# set source files
LOCAL_SRC_FILES :=  \
                  ../../src/api.c \
                  ../../src/dumper.c \
                  ../../src/emitter.c \
                  ../../src/loader.c \
                  ../../src/parser.c \
                  ../../src/reader.c \
                  ../../src/scanner.c \
                  ../../src/writer.c \

LOCAL_CFLAGS := -g -O0

# build static library
include $(BUILD_STATIC_LIBRARY)
