#!/usr/bin/env python2.6

import os;
import sys;
import os.path;
import pickle;
import zlib;
import string;
import sys;
import subprocess;
import platform;
import re;

excludeDirs = ["Box2D", "Freetype", "FileSystem/Yaml", ]
includePaths = {}




def visit_directory(arg, dirname, names):
	global excludeDirs, includePaths
	# (path, name) = os.path.split(dirname);
	if (string.find(dirname, "$process") != -1):
		return;
	if (string.find(dirname, ".svn") != -1):
		return;
	relPath = os.path.relpath(dirname)
	for exDir in excludeDirs:	
		if (string.find(relPath, exDir) == 0):
			#print string.find(relPath, exDir)
			return;
	print relPath

	(dirhead, dirtail) = os.path.split(dirname);
	fullpath = os.path.normpath( dirname + "/");
	for fullname in names:
		pathname = fullpath + "/" + fullname;
		if os.path.isdir(pathname): 
			continue;
		if fullname[0] == '.' or fullname[0] == '$':
			continue;
		includePaths[fullname] = os.path.relpath(pathname);		
		supported_exts = [".cpp", ".h", ".hpp"];
	return
	
def process_contents(contents):
	global includePaths;
	includeFind = re.compile(r"^\s*#\s*include\s*\"([\w./\\]+)\"\s*$", re.MULTILINE)
	matchCount = 0
	pos = 0
	while (True):
		match = includeFind.search(contents, pos)
		if match == None:
			break;
		
		# print match.group(0), 
		
		value = match.group(1);
		valueStart = match.start(1)
		valueEnd = match.end(1)
		
		slash = string.rfind(value, "/");
		if slash != -1:
			value = value[slash + 1:]

		slash = string.rfind(value, "\\");
		if slash != -1:
			value = value[slash + 1:]
		
		if value in includePaths:
			contents = contents[:match.start(1)] + includePaths[value] + contents[match.end(1):];
			pos = match.start(1) + len(includePaths[value]) + 1;
			matchCount = matchCount + 1
		else:
			pos = match.end(0)
	
	# print contents;
	return (contents, matchCount);
	
def process_file(fullname):
	f = open(fullname)
	contents = "";
	try:
		contents = f.read();
	finally:
	    f.close()
	
	(contents, matchCount) = process_contents(contents);
	print fullname, matchCount
	
	f = open(fullname, "wt")
	try: 
		f.write(contents);
	finally:
		f.close();
	
	return;
	
def process_files(arg, dirname, names):
	global excludeDirs, includePaths
	# (path, name) = os.path.split(dirname);
	if (string.find(dirname, "$process") != -1):
		return;
	if (string.find(dirname, ".svn") != -1):
		return;
	relPath = os.path.relpath(dirname)
	for exDir in excludeDirs:	
		if (string.find(relPath, exDir) == 0):
			#print string.find(relPath, exDir)
			return;
	print relPath

	(dirhead, dirtail) = os.path.split(dirname);
	fullpath = os.path.normpath( dirname + "/");
	for fullname in names:
		pathname = fullpath + "/" + fullname;
		if os.path.isdir(pathname): 
			continue;
		if fullname[0] == '.' or fullname[0] == '$':
			continue;
		
		(name, ext) = os.path.splitext(fullname); 
		supported_exts = [".cpp", ".h", ".hpp"];
		if ext in supported_exts:
			process_file(pathname);
			
	return

export_script_dir = os.getcwd();
os.path.walk(export_script_dir, visit_directory, None);
os.path.walk(export_script_dir, process_files, None);


#process_file("Animation/AnimatedObject.cpp")