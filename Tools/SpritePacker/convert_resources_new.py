#!/usr/bin/env python
#
#  convert_resources.py
#
#  Created by Vitaliy Borodovsky on 10/27/08.
#  Copyright (c) 2008 DAVA Consulting, LLC. All rights reserved.
#

# TODO remove data_directory argument
# TODO replace all spawn() and subprocess.call() with exec_tool
# TODO add support for --psd_tool option to not use photoshop

import os
import sys
import os.path
import pickle
import zlib
import string
import sys
import subprocess
import platform

# TODO use standard way of parsing options, although the options are passed to texturepack utility anyway
#import optparse

# global variables

DEBUG_OUTPUT = True
EXT_DEBUG_OUTPUT = True
NO_RUN = True # -- do not execute commands, only debug output

# Conversion mode for texturepacker
#NORMAL = single texture containing all sprites,  SPLIT = every psd = every texture

CONVERSION_NORMAL = "normal"
CONVERSION_SPLIT = "split"

# TODO NOTE conversion_type is not actually used by the texturepacker, only --split option is used because
# all the options passed to this script are passed to the texturepacker utility
conversion_type = None
needRepack = None
needRebuild = None
flags = None
unpacked_resources_directory = None
packed_resources_directory = None
exclude_resources_directory = None


# dictionary to hold CRC data about files 
#export_script_dir = os.getcwd()

# -----------------
# --- functions ---
# -----------------

def exec_tool(tool_exe, tool_args_line):
    """tool_exe = path and name of the executable,
    tool_args_line = arguments as a string"""
    
    cmdline = ' '.join(tool_exe, tool_args_line)
    if DEBUG_OUTPUT:
        print("Executing '{0}'...".format(cmdline))
        
    if not NO_RUN:
        os.system(cmdline)


# -----
def delete_dir_with_contents(top):
    try: 
        for name in os.listdir(top):
            filepath = os.path.join(top, name)
            if (string.find(filepath, ".svn") != -1):
                continue
            if os.path.isdir(filepath): 
                continue;
            os.remove(filepath)
    except OSError, (errno, str_error):
        print str_error, errno
        pass;
#
# we do not need to delete directories because of .svn / and subdirectories must be there after deletion of contents
#       for name in dirs:
#           os.rmdir(os.path.join(root, name))
    return;

# -----
def read_file_as_list(pathname):
    try:
        f = file(pathname, "rb");
        contents = f.read();
        
        f.close();
        f = None;
        
        items = string.split(contents, " ");
        return filter(lambda x: (len(x) > 0) and (x[0] == '-'), items);
    
    except IOError:
        return [];
    
    return [];


# ----
def file_crc(pathname):
    try:
        f = file(pathname, "rb");
        contents = f.read();
        
        return "%d" % (zlib.crc32(contents));
        
        f.close();
        f = None;
    except IOError:
        return "";
    
    return "";


# -----
def write_crc(crcfilename, crc):
    if (DEBUG_OUTPUT): 
        print("-- write_crc: %s / %s" % (crcfilename, crc));
    
    try:
        f = file(crcfilename, "wb");
        f.write(crc);
        f.close();
        f = None;
    except IOError:
        if (DEBUG_OUTPUT):
            print("*** write_crc failed");
        return;
    return;
    
   
# -----
def compare_crc(crcfilename, crc):
    try:
        f = file(crcfilename, "rb");
        contents = f.read();
        if (contents == crc):
            return True;
            
        f.close();
        f = None;
    except IOError:
        return False;
    return False;


# -----
def visit_directory(arg, dirname, names):
    global conversion_type, needRepack, needRebuild, flags;
    
    # TODO remove this evil hack!
    PACKED_RESOURCES_DIR_INDEX = 3
    
    
    # (path, name) = os.path.split(dirname);
    if (string.find(dirname, "$process") != -1):
        return;
    if (string.find(dirname, ".svn") != -1):
        return;

    (dirhead, dirtail) = os.path.split(dirname);
    
    dirtail_withhie = dirname
    dirtail_withhie = string.replace(dirtail_withhie, unpacked_resources_directory, "")

    if dirname == "." or dirname == unpacked_resources_directory:
        defFilesList = ["./texturepack", conversion_type, exclude_resources_directory, packed_resources_directory + "/", dirname + "/"] + flags;
    else:
        defFilesList = ["./texturepack", conversion_type, exclude_resources_directory, packed_resources_directory + dirtail_withhie + "/", dirname + "/"] + flags;

    if (DEBUG_OUTPUT):
        print "-- Process Directory: " + dirname;
        #print "DIR HEAD: " + dirhead;
        #print "DIR TAIL: " + dirtail;
        #print "DEF FILE LIST: ", defFilesList
        #print "PATH REMOVE HEAD:", tmp
    
    somethingChanged = False
    atLeastOneFileExists = False
    
    fullpath = os.path.normpath( dirname + "/");
    
    if (needRebuild):
        delete_dir_with_contents(fullpath + "/$process/");
        
    for fullname in names:
        pathname = fullpath + "/" + fullname;
        if os.path.isdir(pathname): 
            continue;

        if fullname[0] == '.' or fullname[0] == '$':
            continue;
            
        # print "*** Process Path:" + pathname + " Fullname: " + fullname;
                
        (name, ext) = os.path.splitext(fullname); 
        
        supported_exts = [".psd"];
        
        if ext in supported_exts:
            atLeastOneFileExists = True
            crc = file_crc(pathname);
            
            #if (not crc_dict.has_key(pathname)) or (crc_dict[pathname] != crc):
            #   crc_dict[pathname] = crc;
            #   os.spawnv(os.P_WAIT, "./export.sh", ["", fullpath + "/", fullname]);
            #   print "convert: %s crc: %d" % (fullname, crc);
            #   somethingChanged = True
            
            if (compare_crc(fullpath + "/$process/" + fullname + ".crc", crc) == False):
                if (platform.system() == "Darwin"):
                    os.spawnv(os.P_WAIT, "./export.sh", ["", fullpath + "/", fullname]);
                elif (platform.system() == "Windows"):
                    subprocess.call(["cscript.exe"]+["export.vbs", fullpath + "\\", fullname]);
                else:
                    print "*** Error: OS is not supported"
                
                print "convert: %s crc: %s" % (fullname, crc);
                somethingChanged = True
                write_crc(fullpath + "/$process/" + fullname + ".crc", crc);
                        
            defFilesList.append(fullpath + "/$process/" + name + ".txt");
            print "Check:" + fullpath + "/$process/" + name + ".txt"
        
        if ext == ".pngdef":
            # TODO create $process
            print "*** Mark: Found png resource: ";
            # crc = file_crc(fullpath + "/" + name + ".png");
            defFilesList.append(fullpath + "/" + fullname)
            somethingChanged = True
            atLeastOneFileExists = True
        
    texturePackFlags = read_file_as_list(fullpath + "/flags.txt");
    # forms argument list from flags.txt, see wiki
    if (texturePackFlags.count("--rebuild_this") > 0):
        somethingChanged = True;
        
    if atLeastOneFileExists and (somethingChanged or needRepack):
        
        delete_dir_with_contents(defFilesList[PACKED_RESOURCES_DIR_INDEX]);
        try:
            print "* Creating Dir: ", defFilesList[PACKED_RESOURCES_DIR_INDEX]
            os.makedirs(defFilesList[PACKED_RESOURCES_DIR_INDEX]);
            # TODO create $process in DataSource
        except OSError, (errno, str_error):
            print str_error, errno
            pass;
            
        if (DEBUG_OUTPUT):
            print "-- Flags:", texturePackFlags;
            
        if (EXT_DEBUG_OUTPUT):
            print "-- Run texture pack:", defFilesList;
            
        os.spawnv(os.P_WAIT, "./texturepack", defFilesList + texturePackFlags);

        # /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -e PVRTC -p tex5_p.png -o texture5.pvr -f PVR texture5.png
        # if we have --pvr flag set for directory pack textures
        if (texturePackFlags.count("--pvr") > 0):
            print "PVR PACKING"
            for filename in os.listdir(defFilesList[PACKED_RESOURCES_DIR_INDEX]):
                basename, extension = filename.split('.')
                print "Pack:" + basename + "ext:" + extension
                # for every png file in directory
                if (extension == "pvr"):    
                    # set pathnames for preview, pvr output and input png
                    fullPngTexturePath = defFilesList[PACKED_RESOURCES_DIR_INDEX] + filename;
                    fullPvrTexturePath = defFilesList[PACKED_RESOURCES_DIR_INDEX]  + basename + ".pvr";
                    fullPngPreviewTexturePath = defFilesList[PACKED_RESOURCES_DIR_INDEX]  + basename + "_preview.png";
                    args = ["/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool", "-e", "PVRTC", "-p", fullPngPreviewTexturePath, "-o", fullPvrTexturePath, "-f", "PVR", fullPngTexturePath];
                    print "Make PVR texture from: ", filename, " run args: ", args;
                    os.spawnv(os.P_WAIT, "/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool", args);
        
        
    return
    

# ----------------------------
# ---- PROGRAM ENTRY POINT ---
def main():
    global flags, conversion_type, unpacked_resources_directory
    global packed_resources_directory, exclude_resources_directory
    
    
    conversion_type = CONVERSION_NORMAL
    needRepack = False
    needRebuild = False
  
    
    params = sys.argv[1:]
    flags = filter(lambda x: x[0] == '-', params)
    other_params = filter(lambda x: x[0] != '-', params)
    
    
    # no amount matters
    if (flags.count("--split") > 0):
        conversion_type = CONVERSION_SPLIT # NORMAL = single texture containing all sprites,  SPLIT = every psd = every texture
        
    if (flags.count("--repack") > 0): # RUN TEXTURE PACKER forced
        needRepack = True
    if (flags.count("--rebuild") > 0): # forced run photoshop etc.
        needRebuild = True
    
    if (len(other_params) >= 2):
        unpacked_resources_directory = other_params[0] # DataSource/Gfx sources = .psd files
        packed_resources_directory = other_params[1] # Dats/XGFX
        
        data_directory = ""
        if len(other_params) > 2:
            data_directory = other_params[2] # /DataSource = seems to be deprecated
            print "Warning! data_directory argument is deprecated."

        (exclude_resources_directory, not_used) = os.path.split( packed_resources_directory )
        print ">> Input dir: ", unpacked_resources_directory
        print ">> Output dir: ", packed_resources_directory
        print ">> Data dir: ", data_directory
        print ">> Exclude dir: ", exclude_resources_directory
    
        print ">> Convertation of resources started";
        os.path.walk(unpacked_resources_directory, visit_directory, None)
        print ">> Convertation of resources ended"
    else:
        print "Usage: convert_resources.py [unpacked_resources_directory] [packed_resources_directory] [data_directory]"
        print "\t[unpacked_resources_directory] - path where we get the data"
        print "\t[packed_resources_directory] - path we store converted graphics to"
        print "\t[data_directory] - all paths are relative to this directory"
        print "Flags:"
        print "\t--split - put each PSD file to separate texture"
        print "\t--repack - repeat packing of the textures"
        print "\t--rebuild - rebuild all textures"
        
        
# -------
main()
