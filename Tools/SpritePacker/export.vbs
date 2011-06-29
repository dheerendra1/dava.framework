Set vbsArguments = WScript.Arguments

set wshell = CreateObject("wscript.shell")


Dim appRef 
Dim javaScriptFile 

Set appRef = CreateObject( "Photoshop.Application" ) 

appRef.BringToFront 

javaScriptFile = wshell.currentdirectory + "\SpriteExport_win.jsx" 
wscript.echo javaScriptFile

call appRef.DoJavaScriptFile(javaScriptFile, Array(vbsArguments(0), vbsArguments(1)), 1)