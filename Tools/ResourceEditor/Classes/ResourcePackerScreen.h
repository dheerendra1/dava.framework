/*
 *  TestScreen.h
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __DAVAENGINE_RESOURCEPACKERSCREEN_H__
#define __DAVAENGINE_RESOURCEPACKERSCREEN_H__


#include "DAVAEngine.h"
#include "TexturePacker/DefinitionFile.h"
#include "UISpriteEditor.h"
#include "UIFileTree.h"

using namespace DAVA;

class ResourcePackerScreen : public UIScreen, public UIFileTreeDelegate
{
public:
	virtual void LoadResources();
	virtual void UnloadResources();
	virtual void WillAppear();
	virtual void WillDisappear();

	virtual void Draw(const UIGeometricData &geometricData);
	virtual void Input(UIEvent * touch);
	
	// Packing of resources section
	void PackResources();
	
	String inputGfxDirectory;
	String outputGfxDirectory;
	String excludeDirectory;
	String gfxDirName;

	void RecursiveTreeWalk(const String & inputPath,const String & outputPath);
	bool IsMD5ChangedDir(const String & processDirectoryPath, const String & pathname, const String & psdName, bool isRecursive);
	bool IsMD5ChangedFile(const String & processDirectoryPath, const String & pathname, const String & psdName);
	DefinitionFile * ProcessPSD(const String & processDirectoryPath, const String & psdPathname, const String & psdName);
	void ProcessFlags(const String & flagsPathname);
	String GetProcessFolderName();
	String currentFlags;

	// Resource Tree
	UIFileTree * resourceTree;
	void OnCellSelected(UIFileTree * tree, UIFileTreeCell *selectedCell);

	// Sprite Editor Logic
	void OpenSpriteEditor(const String & spriteName);
	UISpriteEditor * spriteEditor;

	
	bool isGfxModified;
};

#endif // __DAVAENGINE_RESOURCEPACKERSCREEN_H__
