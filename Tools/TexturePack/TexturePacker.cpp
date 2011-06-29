/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "ImagePacker.h"
#include "TexturePacker.h"
#include "PngImage.h"
#include "CommandLineParser.h" 

#ifdef WIN32
#define snprintf _snprintf
#endif

TexturePacker::TexturePacker()
{
	maxTextureSize = 1024;
	if (CommandLineParser::Instance()->IsFlagSet("--tsize2048"))
	{
		maxTextureSize = 2048;
	}
}

bool TexturePacker::TryToPack(const Rect2i & textureRect, std::list<DefinitionFile*> & defsList)
{
	printf("%d x %d, ", textureRect.dx, textureRect.dy);
	ImagePacker * packer = new ImagePacker(textureRect);
	
//	for (std::list<DefinitionFile*>::iterator dfi = defsList.begin(); dfi != defsList.end(); ++dfi)
//	{
//		DefinitionFile * defFile = *dfi;
//		for (int frame = 0; frame < defFile->frameCount; ++frame)
//		{
//			if (!packer->AddImage(Size2i::Make(defFile->frameRects[frame].dx, defFile->frameRects[frame].dy), &defFile->frameRects[frame]))
//			{
//				SafeDelete(packer);
//				return false;
//			}
//		}
//	}

	// Packing of sorted by size images
	for (int i = 0; i < sortVector.size(); ++i)
	{
		DefinitionFile * defFile = sortVector[i].defFile;
		int frame = sortVector[i].frameIndex;
		if (!packer->AddImage(Size2i::Make(defFile->frameRects[frame].dx, defFile->frameRects[frame].dy), &defFile->frameRects[frame]))
		{
			SafeDelete(packer);
			return false;
		}
		printf("p: %s %d\n",defFile->filename.c_str(), frame);
	}
	printf("\n* %d x %d - success\n", textureRect.dx, textureRect.dy);
	
	if (lastPackedPacker)
	{
		SafeDelete(lastPackedPacker);
	}
	
	lastPackedPacker = packer;
	return true;
}

int TexturePacker::TryToPackFromSortVector(ImagePacker * packer,std::vector<SizeSortItem> & tempSortVector)
{
	int packedCount = 0;
	// Packing of sorted by size images
	for (int i = 0; i < tempSortVector.size(); ++i)
	{
		DefinitionFile * defFile = tempSortVector[i].defFile;
		int frame = tempSortVector[i].frameIndex;
		if (packer->AddImage(Size2i::Make(defFile->frameRects[frame].dx, defFile->frameRects[frame].dy), &defFile->frameRects[frame]))
		{
			packedCount++;
			tempSortVector.erase(tempSortVector.begin() + i);
			i--;
		}
	}
	return packedCount;
}

float TexturePacker::TryToPackFromSortVectorWeight(ImagePacker * packer,std::vector<SizeSortItem> & tempSortVector)
{
	float weight = 0.0f;
	
	// Packing of sorted by size images
	for (int i = 0; i < tempSortVector.size(); ++i)
	{
		DefinitionFile * defFile = tempSortVector[i].defFile;
		int frame = tempSortVector[i].frameIndex;
		if (packer->AddImage(Size2i::Make(defFile->frameRects[frame].dx, defFile->frameRects[frame].dy), &defFile->frameRects[frame]))
		{
			//float weightCoeff = (float)(tempSortVector.size() - i) / (float)(tempSortVector.size());
			weight += (defFile->frameRects[frame].dx * defFile->frameRects[frame].dy);// * weightCoeff;
			tempSortVector.erase(tempSortVector.begin() + i);
			i--;
		}
	}
	return weight;
}


bool sortFn(const SizeSortItem & a, const SizeSortItem & b)
{
	return a.imageSize > b.imageSize;	
}

void TexturePacker::PackToTexturesSeparate(const char * excludeFolder, const char* outputPath, std::list<DefinitionFile*> & defsList)
{
	lastPackedPacker = 0;
	int textureIndex = 0;
	for (std::list<DefinitionFile*>::iterator dfi = defsList.begin(); dfi != defsList.end(); ++dfi)
	{
		sortVector.clear();
		
		DefinitionFile * defFile = *dfi;
		for (int frame = 0; frame < defFile->frameCount; ++frame)
		{
			SizeSortItem sortItem;
			sortItem.imageSize = defFile->frameRects[frame].dx * defFile->frameRects[frame].dy;
			sortItem.defFile = defFile;
			sortItem.frameIndex = frame;
			sortVector.push_back(sortItem);
		}
		std::sort(sortVector.begin(), sortVector.end(), sortFn);

		
		// try to pack for each resolution
		int bestResolution = (maxTextureSize + 1) * (maxTextureSize + 1);
		int bestXResolution, bestYResolution;
		
		printf("* Packing tries started: ");
		
		for (int yResolution = 8; yResolution <= maxTextureSize; yResolution *= 2)
			for (int xResolution = 8; xResolution <= maxTextureSize; xResolution *= 2)
			{
				Rect2i textureRect = Rect2i::Make(0, 0, xResolution, yResolution);
				
				if (xResolution * yResolution < bestResolution)
					if (TryToPack(textureRect, defsList))
					{
						bestResolution = xResolution * yResolution;
						bestXResolution = xResolution;
						bestYResolution = yResolution;
					}
			}
		printf("\n");
		if (bestResolution != (maxTextureSize + 1) * (maxTextureSize + 1))
		{
			char textureNameWithIndex[50];
			sprintf(textureNameWithIndex, "texture%d", textureIndex++);
			std::string textureName = std::string(outputPath) + std::string(textureNameWithIndex);
			printf("* Writing final texture (%d x %d): %s\n", bestXResolution, bestYResolution , textureName.c_str());
			
			PngImage finalImage;
			finalImage.Create(bestXResolution, bestYResolution);
			
			// Writing 
			for (int frame = 0; frame < defFile->frameCount; ++frame)
			{
				Rect2i *destRect = lastPackedPacker->SearchRectForPtr(&defFile->frameRects[frame]);
				if (!destRect)printf("*** ERROR Can't find rect for frame\n");
				
				char name[256];
				std::string withoutExt = defFile->filename.substr(0, defFile->filename.length() - 4);
				snprintf(name, 256, "%s%d.png", withoutExt.c_str(), frame);
				
				PngImage image;
				image.Read(name);
				finalImage.DrawImage(destRect->x, destRect->y, &image);
			}
			
			if (!WriteDefinition(excludeFolder, outputPath, textureNameWithIndex, defFile))
			{
				printf("* ERROR: failed to write definition\n");
			}
			char textureExtension[5] = "png";
			if (CommandLineParser::Instance()->IsFlagSet("--pvr"))strcpy(textureExtension, "pvr");
			textureName += std::string(".") + textureExtension;

			finalImage.Write(textureName.c_str());
		}
	}
}

void TexturePacker::PackToTextures(const char * excludeFolder, const char* outputPath, std::list<DefinitionFile*> & defsList)
{
	lastPackedPacker = 0;
	for (std::list<DefinitionFile*>::iterator dfi = defsList.begin(); dfi != defsList.end(); ++dfi)
	{
		DefinitionFile * defFile = *dfi;
		for (int frame = 0; frame < defFile->frameCount; ++frame)
		{
			SizeSortItem sortItem;
			sortItem.imageSize = defFile->frameRects[frame].dx * defFile->frameRects[frame].dy;
			sortItem.defFile = defFile;
			sortItem.frameIndex = frame;
			sortVector.push_back(sortItem);
		}
	}

//	for (int i = 0; i < sortVector.size(); ++i)
//	{
//		DefinitionFile * defFile = sortVector[i].defFile;
//		int frame = sortVector[i].frameIndex;
//		printf("[SinglePack] before sort: %s frame: %d w:%d h:%d\n", defFile->filename.c_str(), frame, defFile->frameRects[frame].dx, defFile->frameRects[frame].dy);
//	}
	
	std::sort(sortVector.begin(), sortVector.end(), sortFn);

//	for (int i = 0; i < sortVector.size(); ++i)
//	{
//		DefinitionFile * defFile = sortVector[i].defFile;
//		int frame = sortVector[i].frameIndex;
//		printf("[SinglePack] after sort: %s frame: %d w:%d h:%d\n", defFile->filename.c_str(), frame, defFile->frameRects[frame].dx, defFile->frameRects[frame].dy);
//	}
	
	// try to pack for each resolution
	int bestResolution = (maxTextureSize + 1) * (maxTextureSize + 1);
	int bestXResolution, bestYResolution;
	
	printf("* Packing tries started: ");
	
	bool isPvr = false;
	if (CommandLineParser::Instance()->IsFlagSet("--pvr"))
		isPvr = true;
		
	for (int yResolution = 8; yResolution <= maxTextureSize; yResolution *= 2)
		 for (int xResolution = 8; xResolution <= maxTextureSize; xResolution *= 2)
		 {
			 if ((isPvr) && (xResolution != yResolution))continue;
			 
			 Rect2i textureRect = Rect2i::Make(0, 0, xResolution, yResolution);
			 
			 if (xResolution * yResolution < bestResolution)
				 if (TryToPack(textureRect, defsList))
				 {
					 bestResolution = xResolution * yResolution;
					 bestXResolution = xResolution;
					 bestYResolution = yResolution;
				 }
		 }
	printf("\n");
	if (bestResolution != (maxTextureSize + 1) * (maxTextureSize + 1))
	{
		std::string textureName = std::string(outputPath) + std::string("texture");
		printf("* Writing final texture (%d x %d): %s\n", bestXResolution, bestYResolution , textureName.c_str());
	
		PngImage finalImage;
		finalImage.Create(bestXResolution, bestYResolution);
		
		// Writing 
		for (std::list<DefinitionFile*>::iterator dfi = defsList.begin(); dfi != defsList.end(); ++dfi)
		{
			DefinitionFile * defFile = *dfi;
			for (int frame = 0; frame < defFile->frameCount; ++frame)
			{
				Rect2i *destRect = lastPackedPacker->SearchRectForPtr(&defFile->frameRects[frame]);
				if (!destRect)printf("*** ERROR Can't find rect for frame\n");
				
				char name[256];
				std::string withoutExt = defFile->filename.substr(0, defFile->filename.length() - 4);
				snprintf(name, 256, "%s%d.png", withoutExt.c_str(), frame);

				PngImage image;
				image.Read(name);
				finalImage.DrawImage(destRect->x, destRect->y, &image);

				if (CommandLineParser::Instance()->IsFlagSet("--debug"))
				{
					finalImage.DrawRect(*destRect, 0xFF0000FF);
				}
			}
			
			if (!WriteDefinition(excludeFolder, outputPath, "texture", defFile))
			{
				printf("* ERROR: failed to write definition\n");
			}
		}
		char textureExtension[5] = "png";
		if (CommandLineParser::Instance()->IsFlagSet("--pvr"))strcpy(textureExtension, "pvr");

		textureName += std::string(".") + textureExtension;
		finalImage.Write(textureName.c_str());
	}else
	{
		// 
		PackToMultipleTextures(excludeFolder, outputPath, defsList);
	}
}

void TexturePacker::PackToMultipleTextures(const char * excludeFolder, const char* outputPath, std::list<DefinitionFile*> & defList)
{
	if (defList.size() != 1)
		printf("* ERROR: failed to pack to multiple textures\n");

	for (int i = 0; i < sortVector.size(); ++i)
	{
		DefinitionFile * defFile = sortVector[i].defFile;
		int frame = sortVector[i].frameIndex;
		printf("[MultiPack] prepack: %s frame: %d w:%d h:%d\n", defFile->filename.c_str(), frame, defFile->frameRects[frame].dx, defFile->frameRects[frame].dy);
	}
	
	std::vector<ImagePacker*> & packers = usedPackers;
	
	/*
	// OLD PACKING ALGORITHM
	ImagePacker * currentPacker = new ImagePacker(Rect2i::Make(0, 0, 1024, 1024));
	packers.push_back(currentPacker);

	// Packing of sorted by size images
	for (int i = 0; i < sortVector.size(); ++i)
	{
		DefinitionFile * defFile = sortVector[i].defFile;
		int frame = sortVector[i].frameIndex;
		printf("[MultiPack] pack: %s frame: %d w:%d h:%d\n", defFile->filename.c_str(), frame, defFile->frameRects[frame].dx, defFile->frameRects[frame].dy);
	
		bool packed = false;
		for (int packerIndex = 0; packerIndex < packers.size(); ++packerIndex)
		{
			if (packers[packerIndex]->AddImage(Size2i::Make(defFile->frameRects[frame].dx, defFile->frameRects[frame].dy), &defFile->frameRects[frame]))
			{
				packed = true;
				printf("[MultiPack] added to packer: %d\n", packerIndex);
				break;
			}
		}
		
		if (!packed)
		{
			currentPacker = new ImagePacker(Rect2i::Make(0, 0, 1024, 1024));
			packers.push_back(currentPacker);
			
			if (!currentPacker->AddImage(Size2i::Make(defFile->frameRects[frame].dx, defFile->frameRects[frame].dy), &defFile->frameRects[frame]))
			{
				printf("*** FATAL ERROR: image is too big: imageSize: %d, %d\n", defFile->frameRects[frame].dx, defFile->frameRects[frame].dy);
				return;
			}else
			{
				printf("[MultiPack] added to packer: %d\n", packers.size() - 1);
			}
		}
		
	} */
	
	
	/* // ALGO #2
	std::vector<SizeSortItem> sortVectorWork = sortVector;
		
	while(sortVectorWork.size() > 0)
	{
		// try to pack for each resolution
		int maxPackedObjects = 0;
		//int bestResolution = 1025 * 1025;
		
		printf("* Packing tries started: ");
		
		ImagePacker * bestPackerForThisStep = 0;
		std::vector<SizeSortItem> newWorkVector;

		for (int yResolution = 8; yResolution <= 1024; yResolution *= 2)
			for (int xResolution = 8; xResolution <= 1024; xResolution *= 2)
			{
				Rect2i textureRect = Rect2i::Make(0, 0, xResolution, yResolution);
				ImagePacker * packer = new ImagePacker(textureRect);
				
				std::vector<SizeSortItem> tempSortVector = sortVectorWork;
				int n = TryToPackFromSortVector(packer, tempSortVector);
				
				if (n > maxPackedObjects) 
				{
					maxPackedObjects = n;
					SafeDelete(bestPackerForThisStep);
					bestPackerForThisStep = packer;
					newWorkVector = tempSortVector;
				}
			}
		
		sortVectorWork = newWorkVector;
		packers.push_back(bestPackerForThisStep);
	} */
	
	std::vector<SizeSortItem> sortVectorWork = sortVector;
	
	while(sortVectorWork.size() > 0)
	{
		// try to pack for each resolution
		float maxValue = 0.0f;
		//int bestResolution = 1025 * 1025;
		
		printf("* Packing tries started: ");
		
		ImagePacker * bestPackerForThisStep = 0;
		std::vector<SizeSortItem> newWorkVector;
		
		for (int yResolution = 8; yResolution <= maxTextureSize; yResolution *= 2)
			for (int xResolution = 8; xResolution <= maxTextureSize; xResolution *= 2)
			{
				if (CommandLineParser::Instance()->IsFlagSet("--pvr") && (xResolution != yResolution))continue;
				
				Rect2i textureRect = Rect2i::Make(0, 0, xResolution, yResolution);
				ImagePacker * packer = new ImagePacker(textureRect);
				
				std::vector<SizeSortItem> tempSortVector = sortVectorWork;
				float n = TryToPackFromSortVectorWeight(packer, tempSortVector);
				
				if (n > maxValue) 
				{
					maxValue = n;
					SafeDelete(bestPackerForThisStep);
					bestPackerForThisStep = packer;
					newWorkVector = tempSortVector;
				}
			}
		
		sortVectorWork = newWorkVector;
		packers.push_back(bestPackerForThisStep);
	}
	
	printf("* Writing %d final textures \n", (int)packers.size());

	std::vector<PngImage*> finalImages;
	
	for (int imageIndex = 0; imageIndex < packers.size(); ++imageIndex)
	{
		PngImage * image = new PngImage();
		ImagePacker * packer = packers[imageIndex];
		image->Create(packer->GetRect().dx, packer->GetRect().dy);
		finalImages.push_back(image);
	}
	
	for (std::list<DefinitionFile*>::iterator defi = defList.begin(); defi != defList.end(); ++defi)
	{
		DefinitionFile * defFile = *defi;
		
		for (int frame = 0; frame < defFile->frameCount; ++frame)
		{
			Rect2i * destRect;
			ImagePacker * foundPacker = 0;
			int packerIndex = 0;
			char name[256];
			
			for (packerIndex = 0; packerIndex < packers.size(); ++packerIndex)
			{
				destRect = packers[packerIndex]->SearchRectForPtr(&defFile->frameRects[frame]);
			
				if (destRect)
				{
					foundPacker = packers[packerIndex];
					std::string withoutExt = defFile->filename.substr(0, defFile->filename.length() - 4);
					snprintf(name, 256, "%s%d.png", withoutExt.c_str(), frame);
					break;
				}
			}
			
			if (foundPacker)
			{
				printf("[MultiPack] pack to texture: %d\n", packerIndex);
				PngImage image;
				image.Read(name);
				finalImages[packerIndex]->DrawImage(destRect->x, destRect->y, &image);
				if (CommandLineParser::Instance()->IsFlagSet("--debug"))
				{
					finalImages[packerIndex]->DrawRect(*destRect, 0xFF0000FF);
				}
			}
		}
	}
	
	for (int image = 0; image < packers.size(); ++image)
	{
		char temp[256];
		sprintf(temp, "texture%d.png", image);
		std::string textureName = std::string(outputPath) + std::string(temp);
		finalImages[image]->Write(textureName.c_str());
	}	

	for (std::list<DefinitionFile*>::iterator defi = defList.begin(); defi != defList.end(); ++defi)
	{
		DefinitionFile * defFile = *defi;
		
		std::string textureName = std::string(outputPath) + std::string("texture");
		if (!WriteMultipleDefinition(excludeFolder, outputPath, "texture", defFile))
		{
			printf("* ERROR: failed to write definition\n");
		}
	}	
}

void replace(std::string & repString,const std::string & needle, const std::string & s)
{
	std::string::size_type lastpos = 0, thispos;
	while ((thispos = repString.find(needle, lastpos)) != std::string::npos)
	{
		repString.replace(thispos, needle.length(), s);
		lastpos = thispos + 1;
	}
}

Rect2i TexturePacker::ReduceRectToOriginalSize(const Rect2i & _input)
{
	Rect2i r = _input;
	if (CommandLineParser::Instance()->IsFlagSet("--add0pixel"))
	{
	}
	else if (CommandLineParser::Instance()->IsFlagSet("--add1pixel"))
	{
		r.dx--;
		r.dy--;
	}
	else if (CommandLineParser::Instance()->IsFlagSet("--add2pixel"))
	{
		r.dx-=2;
		r.dy-=2;
	}
	else if (CommandLineParser::Instance()->IsFlagSet("--add4pixel"))
	{
		r.dx-=4;
		r.dy-=4;
	}else		// add 1 pixel by default
	{
		r.dx--;
		r.dy--;
	}
	return r;
}

bool TexturePacker::WriteDefinition(const char * excludeFolder, const char * outputPath, const char * _textureName, DefinitionFile * defFile)
{
	const char * fileName = strrchr(defFile->filename.c_str(), '/');
	printf("* Write definition: %s\n", fileName);
	
	std::string defFilePath = std::string(outputPath) + std::string(fileName);
	FILE * fp = fopen(defFilePath.c_str(), "wt");
	if (!fp)return false;
	
	std::string textureName = _textureName;
//	replace(textureName, std::string(excludeFolder), std::string(""));
	
	fprintf(fp, "%d\n", 1);

	char textureExtension[5] = "png";
	if (CommandLineParser::Instance()->IsFlagSet("--pvr"))
		strcpy(textureExtension, "pvr");
	
	fprintf(fp, "%s.%s\n", textureName.c_str(), textureExtension);
	
	fprintf(fp, "%d %d\n", defFile->spriteWidth, defFile->spriteHeight);
	fprintf(fp, "%d\n", defFile->frameCount); 
	for (int frame = 0; frame < defFile->frameCount; ++frame)
	{
		Rect2i *destRect = lastPackedPacker->SearchRectForPtr(&defFile->frameRects[frame]);
		Rect2i origRect = defFile->frameRects[frame];
		Rect2i writeRect = ReduceRectToOriginalSize(*destRect);
		fprintf(fp, "%d %d %d %d %d %d %d\n", writeRect.x, writeRect.y, writeRect.dx, writeRect.dy, origRect.x, origRect.y, 0);
	}
	
	for (int pathInfoLine = 0; pathInfoLine < defFile->pathsInfo.size(); ++pathInfoLine)
	{
		std::string & line = defFile->pathsInfo[pathInfoLine];
		fprintf(fp, "%s", line.c_str());
	}
	
	fclose(fp);
	return true;
}

bool TexturePacker::WriteMultipleDefinition(const char * excludeFolder, const char * outputPath, const char * _textureName, DefinitionFile * defFile)
{
	const char * fileName = strrchr(defFile->filename.c_str(), '/');
	printf("* Write definition: %s\n", fileName);
	
	std::string defFilePath = std::string(outputPath) + std::string(fileName);
	FILE * fp = fopen(defFilePath.c_str(), "wt");
	if (!fp)return false;
	
	std::string textureName = _textureName;
//	replace(textureName, std::string(excludeFolder), std::string(""));
	
	char textureExtension[5] = "png";
	if (CommandLineParser::Instance()->IsFlagSet("--pvr"))
		strcpy(textureExtension, "pvr");
	
	std::vector<int> packerIndexArray;
	packerIndexArray.resize(defFile->frameCount);
	
	std::map<int, int> packerIndexToFileIndex;
	
	// find used texture indexes for this sprite
	for (int frame = 0; frame < defFile->frameCount; ++frame)
	{
		Rect2i * destRect = 0;
		int packerIndex = 0;
		for (packerIndex = 0; packerIndex < usedPackers.size(); ++packerIndex)
		{
			destRect = usedPackers[packerIndex]->SearchRectForPtr(&defFile->frameRects[frame]);
			if (destRect)break;
		}
		// save packer index for frame
		packerIndexArray[frame] = packerIndex;
		// add value to map to show that this packerIndex was used
		packerIndexToFileIndex[packerIndex] = -1;
	}
		
	// write real used packers count
	fprintf(fp, "%d\n", (int)packerIndexToFileIndex.size());
	
	int realIndex = 0;
	// write user texture indexes
	for (int i = 0; i < usedPackers.size(); ++i)
	{
		std::map<int, int>::iterator isUsed = packerIndexToFileIndex.find(i);
		if (isUsed != packerIndexToFileIndex.end())
		{
			// here we write filename for i-th texture and write to map real index in file for this texture
			fprintf(fp, "%s%d.%s\n", textureName.c_str(), i, textureExtension);
			packerIndexToFileIndex[i] = realIndex++;
		}
	}
	
	fprintf(fp, "%d %d\n", defFile->spriteWidth, defFile->spriteHeight);
	fprintf(fp, "%d\n", defFile->frameCount); 
	for (int frame = 0; frame < defFile->frameCount; ++frame)
	{
		Rect2i * destRect = 0;
		for (int packerIndex = 0; packerIndex < usedPackers.size(); ++packerIndex)
		{
			destRect = usedPackers[packerIndex]->SearchRectForPtr(&defFile->frameRects[frame]);
			if (destRect)break;
		}
		int packerIndex = packerIndexToFileIndex[packerIndexArray[frame]]; // here get real index in file for our used texture
		if (destRect)
		{
			Rect2i origRect = defFile->frameRects[frame];
			Rect2i writeRect = ReduceRectToOriginalSize(*destRect);
			fprintf(fp, "%d %d %d %d %d %d %d\n", writeRect.x, writeRect.y, writeRect.dx, writeRect.dy, origRect.x, origRect.y, packerIndex);
		}else
		{
			printf("*** FATAL ERROR: can't find rect in all of packers");
		}
	}
	
	for (int pathInfoLine = 0; pathInfoLine < defFile->pathsInfo.size(); ++pathInfoLine)
	{
		std::string & line = defFile->pathsInfo[pathInfoLine];
		fprintf(fp, "%s", line.c_str());
	}
	
	fclose(fp);
	return true;
}



