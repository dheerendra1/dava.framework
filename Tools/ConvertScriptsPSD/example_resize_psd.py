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

dataDir = os.getcwd(); 
print "Scale convert script" + os.getcwd();


os.chdir(dataDir + "/../Tools/ConvertScriptsPSD/");
#Be careful! Script only can replace source Gfx with the scaled one!
params = ["", dataDir + "/Gfx", dataDir +  "/Gfx", "2.0", "2.0"];
print "p:", params;
os.spawnv(os.P_WAIT, "./convert.py", params);
