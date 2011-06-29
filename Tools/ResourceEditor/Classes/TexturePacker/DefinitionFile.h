/*
 *  DefinitionFile.h
 *  texturepack
 *
 *  Created by Vitaliy Borodovsky on 10/28/08.
 *  Copyright 2008 DAVA Consulting, LLC. All rights reserved.
 *
 */

#ifndef __DEFINITION_FILE_H__
#define __DEFINITION_FILE_H__

#include <string>
#include <vector>
#include "DAVAEngine.h"

using namespace DAVA;


class DefinitionFile 
{
public:
	bool Load(const std::string & filename);
	bool LoadPNGDef(const std::string & filename, const std::string & pathToProcess);
	
	DefinitionFile();
	~DefinitionFile();
	
	void ClearPackedFrames();
	
	std::string filename;
	int			frameCount;
	int			spriteWidth;
	int			spriteHeight;
	Rect2i		* frameRects;

	std::vector<std::string> pathsInfo;
};

#endif