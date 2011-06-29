#!/bin/sh 
#
#  convert.sh <Input Directory path> <Output directory path> <Input file name with extension>
#
#	$1 - Input directory path
#	$2 - Output directory path
#	$3 - Input file name with extension
#	$4 - Convertation mode [coords - coordinate mode, percentage - percentage mode] 
#	$5 - X scale value
#	$6 - Y scale value
#
#  Created by Vitaliy Borodovsky on 10/28/08.
#  Copyright (c) 2008 DAVA Consulting, LLC. All rights reserved.
#

echo "Convert params:" $0 $1 $2 $3 $4

osascript << END
tell application "Adobe Photoshop CS3"
	set exportScriptPath to a reference to file "`pwd`/ConvertScript.jsx" 
	do javascript exportScriptPath with arguments {"$1", "$2", "$3", "$4"}
end tell
END

##
## end of script
## 