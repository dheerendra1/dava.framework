#!/usr/bin/env python2.5
#
#  convert_resources.py
#
#  Created by Vitaliy Borodovsky on 10/27/08.
#  Copyright (c) 2008 DAVA Consulting, LLC. All rights reserved.
#

import os;
import sys;
import os.path;
import pickle;
import zlib;
import string;
import sys;
import subprocess;

# global variables

DEBUG_OUTPUT = True
EXT_DEBUG_OUTPUT = True

##
## Configuration part
##
unpacked_resources_directory = "/Users/Hottych/Documents/Jojo2/Data/Gfx"
packed_resources_directory = "/Users/Hottych/Documents/Jojo2/Data/XGfx"
data_directory = "********"

#unpacked_resources_directory = "/Sources/Jojo2/Data/Gfx"
#packed_resources_directory = "/Sources/Jojo2/Data/XGfx"

(exclude_resources_directory, not_used) = os.path.split( packed_resources_directory );
##
##
##
conversion_type = "normal";
needRepack = False;

# dictionary to hold CRC data about files 
export_script_dir = os.getcwd();


def delete_dir_with_contents(top):
	for root, dirs, files in os.walk(top, topdown=False):
		for name in files:
			filepath = os.path.join(root, name)
			if (string.find(filepath, ".svn") != -1):
				continue
			os.remove(filepath)
#
# we do not need to delete directories because of .svn / and subdirectories must be there after deletion of contents
#		for name in dirs:
#			os.rmdir(os.path.join(root, name))
	return;


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


PACKED_RESOURCES_DIR_INDEX = 3

def visit_directory(arg, dirname, names):
	global conversion_type, needRepack, flags;
	
	# (path, name) = os.path.split(dirname);
	if (string.find(dirname, "$process") != -1):
		return;
	if (string.find(dirname, ".svn") != -1):
		return;

	(dirhead, dirtail) = os.path.split(dirname);
	
	dirtail_withhie = dirname
	dirtail_withhie = string.replace(dirtail_withhie, unpacked_resources_directory, "")

	if dirname == "." or dirname == unpacked_resources_directory:
		defFilesList = ["./texturepack", conversion_type, exclude_resources_directory, packed_resources_directory + "/", dirname + "/"];
	else:
		defFilesList = ["./texturepack", conversion_type, exclude_resources_directory, packed_resources_directory + dirtail_withhie + "/", dirname + "/"];

	if (DEBUG_OUTPUT):
		print "-- Process Directory: " + dirname;
		#print "DIR HEAD: " + dirhead;
		#print "DIR TAIL: " + dirtail;
		#print "DEF FILE LIST: ", defFilesList
		#print "PATH REMOVE HEAD:", tmp
	
	somethingChanged = False
	atLeastOneFileExists = False
	
	fullpath = os.path.normpath( dirname + "/");
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
			#	crc_dict[pathname] = crc;
			#	os.spawnv(os.P_WAIT, "./export.sh", ["", fullpath + "/", fullname]);
			#	print "convert: %s crc: %d" % (fullname, crc);
			#	somethingChanged = True
			
			if (compare_crc(fullpath + "/$process/" + fullname + ".crc", crc) == False):
				#os.spawnv(os.P_WAIT, "./export.sh", ["", fullpath + "/", fullname]);
				subprocess.call(["cscript.exe"]+["export.vbs", fullpath + "\\", fullname]);
				print "convert: %s crc: %s" % (fullname, crc);
				somethingChanged = True
				write_crc(fullpath + "/$process/" + fullname + ".crc", crc);
						
			defFilesList.append(fullpath + "/$process/" + name + ".txt");
			print "Check:" + fullpath + "/$process/" + name + ".txt"
	
	texturePackFlags = read_file_as_list(fullpath + "/flags.txt");
	if (texturePackFlags.count("-rebuild_this") > 0):
		somethingChanged = True;
		
	if atLeastOneFileExists and (somethingChanged or needRepack):
		
		delete_dir_with_contents(defFilesList[PACKED_RESOURCES_DIR_INDEX]);
		try:
			print "* Creating Dir: ", defFilesList[PACKED_RESOURCES_DIR_INDEX]
			os.makedirs(defFilesList[PACKED_RESOURCES_DIR_INDEX]);
		except OSError, (errno, str_error):
			print str_error, errno
			pass;
			
		if (DEBUG_OUTPUT):
			print "-- Flags:", texturePackFlags;
			
		if (EXT_DEBUG_OUTPUT):
			print "-- Run texture pack:", defFilesList + texturePackFlags;
			
		os.spawnv(os.P_WAIT, "./texturepack", defFilesList + texturePackFlags);

		# /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -e PVRTC -p tex5_p.png -o texture5.pvr -f PVR texture5.png
		# if we have -pvr flag set for directory pack textures
		if (texturePackFlags.count("-pvr") > 0):
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

print ">> Convertation of resources started";
flags = [];
params = sys.argv[1:];
flags = filter(lambda x: x[0] == '-', params);
other_params = filter(lambda x: x[0] != '-', params);

if (len(other_params) >= 3):
	unpacked_resources_directory = other_params[0]
	packed_resources_directory = other_params[1]
	data_directory = other_params[2]
	(exclude_resources_directory, not_used) = os.path.split( packed_resources_directory );
	print ">> Input dir: ", unpacked_resources_directory;
	print ">> Output dir: ", packed_resources_directory;
	print ">> Data dir: ", data_directory;
	print ">> Exclude dir: ", exclude_resources_directory;

if (flags.count("-split") > 0):
	conversion_type = "split";
	
if (flags.count("-repack") > 0):
	needRepack = True;
		
os.path.walk(unpacked_resources_directory, visit_directory, None);


print ">> Convertation of resources ended";

