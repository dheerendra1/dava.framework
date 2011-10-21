#-----------------------------
# XmlLib lib

# set local path for lib
LOCAL_PATH := $(call my-dir)

# clear all variables
include $(CLEAR_VARS)

# set mdule name
LOCAL_MODULE := libxml_android

# set path for includes
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include

# set exported includes
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

# set source files
LOCAL_SRC_FILES :=  \
                  ../../src/c14n.c \
                  ../../src/catalog.c \
                  ../../src/chvalid.c \
                  ../../src/debugXML.c \
                  ../../src/dict.c \
                  ../../src/DOCBparser.c \
                  ../../src/encoding.c \
                  ../../src/entities.c \
                  ../../src/error.c \
                  ../../src/globals.c \
                  ../../src/hash.c \
                  ../../src/HTMLparser.c \
                  ../../src/HTMLtree.c \
                  ../../src/legacy.c \
                  ../../src/list.c \
                  ../../src/parser.c \
                  ../../src/parserInternals.c \
                  ../../src/pattern.c \
                  ../../src/relaxng.c \
                  ../../src/SAX.c \
                  ../../src/SAX2.c \
                  ../../src/schematron.c \
                  ../../src/threads.c \
                  ../../src/tree.c \
                  ../../src/trio.c \
                  ../../src/trionan.c \
                  ../../src/triostr.c \
                  ../../src/uri.c \
                  ../../src/valid.c \
                  ../../src/xinclude.c \
                  ../../src/xlink.c \
                  ../../src/xmlIO.c \
                  ../../src/xmlmemory.c \
                  ../../src/xmlmodule.c \
                  ../../src/xmlreader.c \
                  ../../src/xmlregexp.c \
                  ../../src/xmlsave.c \
                  ../../src/xmlschemas.c \
                  ../../src/xmlschemastypes.c \
                  ../../src/xmlstring.c \
                  ../../src/xmlunicode.c \
                  ../../src/xmlwriter.c \
                  ../../src/xpath.c \
                  ../../src/xpointer.c \

# set build flags
LOCAL_CFLAGS := -DHAVE_CONFIG_H -g -O0

#set exported build flags
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS) 
                    
# build static library
include $(BUILD_STATIC_LIBRARY)
