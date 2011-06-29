/*
 *  TexturePacker.h
 *  texturepack
 *
 *  Created by Vitaliy Borodovsky on 10/28/08.
 *  Copyright 2008 DAVA Consulting, LLC. All rights reserved.
 *
 */
#ifndef __DAVAENGINE_TEXTURE_PACKER_H__
#define __DAVAENGINE_TEXTURE_PACKER_H__

#include <string>
#include <list>
#include <vector>
#include "DefinitionFile.h"
#include "ImagePacker.h"


struct SizeSortItem
{
	int					imageSize;
	DefinitionFile *	defFile;
	int					frameIndex;
};

class TexturePacker 
{
public:
	TexturePacker();
	
	// pack textures to single texture
	void PackToTextures(const char * excludeFolder, const char* outputPath, std::list<DefinitionFile*> & defsList);
	// page each PSD file to separate texture
	void PackToTexturesSeparate(const char * excludeFolder, const char* outputPath, std::list<DefinitionFile*> & defsList);
	// pack one sprite and use several textures if more than one needed
	void PackToMultipleTextures(const char * excludeFolder, const char* outputPath, std::list<DefinitionFile*> & remainingList);

	bool TryToPack(const Rect2i & textureRect, std::list<DefinitionFile*> & defsList);
	bool WriteDefinition(const char * excludeFolder, const char * outputPath, const char * textureName, DefinitionFile * defFile);
	bool WriteMultipleDefinition(const char * excludeFolder, const char * outputPath, const char * _textureName, DefinitionFile * defFile);

	int TryToPackFromSortVector(ImagePacker * packer, std::vector<SizeSortItem> & tempSortVector);
	float TryToPackFromSortVectorWeight(ImagePacker * packer,std::vector<SizeSortItem> & tempSortVector);

	Rect2i ReduceRectToOriginalSize(const Rect2i & _input);
	
private:
	ImagePacker *			lastPackedPacker;
	std::vector<ImagePacker*> usedPackers;

	std::vector<SizeSortItem> sortVector;
	int32 maxTextureSize;
};

#endif // __DAVAENGINE_TEXTURE_PACKER_H__

