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

excludeDirs = ["Box2D", "External\Freetype"]
includePaths = {}
export_script_dir = os.getcwd() + "/../../Sources/";

	
def getbeforeslash(value):
	slash = string.rfind(value, "/");
	if slash != -1:
		value = value[slash + 1:]

	slash = string.rfind(value, "\\");
	if slash != -1:
		value = value[slash + 1:]
	return value;
	

indent = 0
resultString = ""
def visit_directory(dirname, indent):
	global excludeDirs, includePaths, resultString;
	# (path, name) = os.path.split(dirname);
	if (string.find(dirname, "$process") != -1):
		return;
	if (string.find(dirname, ".svn") != -1):
		return;
	relPath = os.path.relpath(dirname, export_script_dir)
	for exDir in excludeDirs:	
		if (string.find(relPath, exDir) == 0):
			#print string.find(relPath, exDir)
			return;


	if (relPath == "."):
		resultString += " " * indent + "<Files>\n"
	else:
		resultString += " " * indent + "<Filter Name=\"%s\">\n" % (getbeforeslash(relPath))

	(dirhead, dirtail) = os.path.split(dirname);
	fullpath = os.path.normpath( dirname + "/");

	for fullname in os.listdir(dirname):
		if os.path.isfile(os.path.join(dirname, fullname)):
			pathname = fullpath + "/" + fullname;
			if os.path.isdir(pathname): 
				continue;
			if fullname[0] == '.' or fullname[0] == '$':
				continue;
			includePaths[fullname] = os.path.relpath(pathname);		
			
			(name, ext) = os.path.splitext(fullname); 
			supported_exts = [".cpp", ".c", ".h", ".hpp"];
			
			if ext in supported_exts:
				resultString = resultString + " " * (indent + 1) + "<File RelativePath=\"%s\"/>\n" % (os.path.relpath(pathname))
			
		elif os.path.isdir(os.path.join(dirname, fullname)):
			visit_directory(os.path.join(dirname, fullname), indent + 1);
			
        # if ext in supported_exts:

	if (relPath == "."):
		resultString += " " * indent + "</Files>\n"
	else:
		resultString = resultString + " " * indent + "</Filter>\n";
	return

def process_contents(contents):
	global includePaths, resultString;
	
	
	p1 = string.rfind(contents, "<Files>");
	p2 = string.rfind(contents, "</Files>");
	
	if p1 != -1 and p2 != -1:
		contents = contents[:p1] + resultString + contents[p2 + 8:];
	
	return contents
	
def process_file(fullname):
	f = open(fullname)
	contents = "";
	try:
		contents = f.read();
	finally:
	    f.close()
	
	contents = process_contents(contents);
	#print contents
	
	f = open(fullname, "wt")
	try: 
		f.write(contents);
	finally:
		f.close();
	
	return;
	

#<Files>
#<Filter Name="Filter">
#<File RelativePath="..\..\SDK\Sources\Animation\AnimatedObject.cpp" /> 
#<File RelativePath="..\..\SDK\Sources\Animation\AnimatedObject.h" /> 
#<Filter Name="Subfilter">
#<File RelativePath="..\..\SDK\Sources\Math\AABBox2.cpp" /> 
#<File RelativePath="..\..\SDK\Sources\Math\AABBox2.h" /> 
#<File RelativePath="..\..\SDK\Sources\Math\AABBox3.cpp" /> 

visit_directory(export_script_dir, 0)
process_file("Win32Framework.vcproj")
