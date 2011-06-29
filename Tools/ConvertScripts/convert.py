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


##
## Configuration part
##

##
conversionMode = "percentage";
conversionXScale = "1";
conversionYScale = "1";

# dictionary to hold CRC data about files 

def visit_directory(arg, dirname, names):
	global conversionMode, conversionXScale, conversionYScale;
	
	(dirhead, dirtail) = os.path.split(dirname);
		
	for fullname in names:
		fullpath = os.path.normpath( dirname + "/");
		pathname = fullpath + "/" + fullname;
		print pathname;
		
		if os.path.isdir(pathname): 
			continue;

		if fullname[0] == '.' or fullname[0] == '$':
			continue;
		
		(name, ext) = os.path.splitext(fullname); 
		
		supported_exts = [".png"];
		
		if ext in supported_exts:
			os.spawnv(os.P_WAIT, "./convert.sh", ["", input_resources_directory, output_resources_directory, name + ext
												, conversionMode, conversionXScale, conversionYScale]);
									
	return

print ">> Convertation of files started";
flags = [];
params = sys.argv[1:];
flags = filter(lambda x: x[0] == '-', params);
params = filter(lambda x: x[0] != '-', params);

if (flags.count("-pixels") > 0):
	conversionMode = "pixels";

if (len(params) >= 4):
	print "%s, %s, %s, %s" % (params[0], params[1], params[2], params[3])

	input_resources_directory = params[0]; #"/Users/Hottych/Documents/Jojo2/Data/RealGfx/MaleHangerClothing"
	output_resources_directory = params[1]; #"/Users/Hottych/Documents/Jojo2/Data/SplitedGfx/MaleHangerClothing"

	conversionXScale = params[2];
	conversionYScale = params[3];

	os.path.walk(input_resources_directory, visit_directory, None);
else:
	print "USAGE: convert.py [path_in] [path_out] [xscale] [yscale]";
print ">> Convertation of files ended";

