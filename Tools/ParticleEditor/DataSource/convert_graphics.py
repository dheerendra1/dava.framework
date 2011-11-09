#!/usr/bin/env python2.5
#
#  convert_graphics.py
#  DAVA SDK
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
import platform;

# *************** HERE YOU SHOULD SETUP YOUR OWN PATHS ***************
framework_path = { "Darwin": "./../../..", "Windows": "./../../.." }
# *************** HERE YOU SHOULD SETUP YOUR OWN PATHS ***************

currentDir = os.getcwd(); 
dataDir =  os.path.realpath(currentDir + "/../Data/")
print "*** DAVA SDK Launching command line packer - data directory:" + dataDir


params = filter(lambda x: x[0] != '-', sys.argv[1:]);
flags = filter(lambda x: x[0] == '-', sys.argv[1:]);

gfxDirs = filter(lambda x: x[0:3] == "Gfx", os.listdir(currentDir));
gfxDirs.extend(params);
# print gfxDirs

if (framework_path[platform.system()] != ""):
    os.chdir(framework_path[platform.system()] + "/Tools/Bin/");
    for dir in gfxDirs:
        params = ["./ResourcePacker", os.path.realpath(currentDir + "/" + dir)] + flags;
        os.spawnv(os.P_WAIT, "./ResourcePacker", params);
else:
	print "Framework path not defined, please define it in dava_framework_path.py"
