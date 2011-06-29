#!/bin/sh 
#
#  export.sh [filepath to export]
#
#  Created by Vitaliy Borodovsky on 10/28/08.
#  Copyright (c) 2008 DAVA Consulting, LLC. All rights reserved.
#
echo "Run coordinate export:" $0 $1

osascript <<END

tell application "Adobe Photoshop CS3"
	set exportScriptPath to a reference to file "`pwd`/CoordExport.jsx" 
	do javascript exportScriptPath with arguments {"`pwd`/", "$1"}
end tell
END

##
## end of script
## 