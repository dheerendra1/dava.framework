# Common functions for Scons scripts...
import os

def FileListAppend(fileList, buildRelativeFolder, includeFolder, wantedExtension):
	fullFolder = os.path.join(buildRelativeFolder, includeFolder)
	for filename in os.listdir(fullFolder):
		if not filename.endswith("." + wantedExtension): continue
		fullFilePathName = os.path.join(buildRelativeFolder, includeFolder, filename)
		if not os.path.isfile(fullFilePathName): continue
		includeFilePathName = os.path.join(includeFolder, filename)
		if includeFilePathName.startswith(".."): includeFilePathName = "#" + includeFilePathName
		fileList.append(includeFilePathName)

def BuildName(isDebug, isUnicode, isShared):
	if int(isDebug): buildName = "Debug"
	else: buildName = "Retail"
	if int(isUnicode): buildName += "Unicode"
	if int(isShared): buildName += "Shared"
	return buildName
