#!/usr/bin/env python2.5
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
import platform

FRAMEWORK_VERSION = "1.0"

print "* DAVA Framework: OS:", platform.system(), platform.release()
print "* Python interpreter: ", sys.executable
print "* This script will perform installation of the framework soon, now it does nothing"
# print "* DAVA Framework: python path:", os.environ['PYTHONPATH']

import os
from os.path import join, splitext, split, exists
from shutil import copyfile

def copy_directory(source, target, exclude = ()):
    if not os.path.exists(target):
        os.makedirs(target)
    for root, dirs, files in os.walk(source):
        if '.svn' in dirs:
            dirs.remove('.svn')  # don't visit .svn directories           
        for file in files:
            if splitext(file)[-1] in exclude: # ('.pyc', '.pyo', '.fs'):
                continue
            from_ = join(root, file)           
            to_ = from_.replace(source, target, 1)
            to_directory = split(to_)[0]
            if not exists(to_directory):
                os.makedirs(to_directory)
            copyfile(from_, to_)

currentDir = os.getcwd(); 
frameworkDir = os.path.realpath(currentDir + "/../");
installDir =  os.path.realpath(frameworkDir + "/../framework.installed/v" + FRAMEWORK_VERSION + "/dava.framework")  

print frameworkDir
print installDir
# print sourceDir
copy_directory(frameworkDir + "/SDK/Sources/", installDir + "/SDK/Sources/", ('.cpp', '.c', '.mm', '.m'));
copy_directory(frameworkDir + "/SDK/External/", installDir + "/SDK/External/");
copy_directory(frameworkDir + "/Tools/Bin/", installDir + "/Tools/Bin/");
copy_directory(frameworkDir + "/Tools/SpritePacker/", installDir + "/Tools/SpritePacker/", ('.cpp', '.c', '.mm', '.m', '.h', '.hpp'));
### copy_directory(frameworkDir + "/Tools/ResourceEditor/", installDir + "/Tools/ResourceEditor/", ('.cpp', '.c', '.mm', '.m', '.h', '.hpp'));

copy_directory(frameworkDir + "/Libs/dxsdk8/", installDir + "/Libs/dxsdk8/");
copy_directory(frameworkDir + "/Libs/oggvorbis/", installDir + "/Libs/oggvorbis/");
copy_directory(frameworkDir + "/Projects/Win32Framework/", installDir + "/Projects/Win32Framework/", ('.ncb', '.o', '.obj', '.pdb', '.idb'));

copy_directory(frameworkDir + "/Projects/TemplateProject/", installDir + "/../TemplateProject/", ('.ncb', '.o', '.obj', '.pdb', '.idb'));


# install dependencies
# copy_directory(frameworkDir + "/Install/Dependencies/", installDir + "/Install/");
#launch = '"%s" %s/p:Configuration=Debug' % (os.path.realpath(os.environ["VS80COMNTOOLS"] + "/../devenv"), frameworkDir + "/Projects/Win32Framework/Win32Framework.sln");
#print launch
#os.system(launch)



