/*
 *  TestScreen.cpp
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ResourcePackerScreen.h"
#include <Magick++.h>
#include <magick/MagickCore.h>
#include <magick/property.h>
#include "TexturePacker/DefinitionFile.h"
#include "TexturePacker/TexturePacker.h"
#include "TexturePacker/CommandLineParser.h"
#include "UIFileTree.h"

void ResourcePackerScreen::LoadResources()
{
	inputGfxDirectory = FileSystem::RealPath("/Sources/dava.framework/Tools/ResourceEditor/DataSource/Gfx/");
    Logger::Debug("%s", inputGfxDirectory.c_str());
	outputGfxDirectory = FileSystem::RealPath(inputGfxDirectory + "/../../Data/Gfx");
	excludeDirectory = FileSystem::RealPath(inputGfxDirectory + "/../");
	
	// PackResources();
	
	UIYamlLoader::Load(this, "~res:/Screens/ResourcePackerScreen.yaml");
	
	resourceTree = SafeRetain(dynamic_cast<UIFileTree*>(FindByName("resourceTree")));
	resourceTree->SetDelegate(this);
	resourceTree->SetFolderNavigation(true);
	resourceTree->SetPath(outputGfxDirectory, ".txt");
	
	// Resource Tree Setup
//	resourceTree = new UIFileTree(Rect(0, 0, 200, 600));
//	resourceTree->SetPath(outputGfxDirectory, ".txt");
//	AddControl(resourceTree);
//	resourceTree->SetDelegate(this);

	//resourceTree->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
	//resourceTree->GetBackground()->SetColor(Color(0.3, 0.3, 0.3, 1.0f));
	
	// SpriteEditor setup
	spriteEditor = SafeRetain(dynamic_cast<UISpriteEditor*> (FindByName("spriteEditor")));
//	spriteEditor = new UISpriteEditor(Rect(220, 0, GetScreenWidth() - 230, 500));
//	spriteEditor->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
//	spriteEditor->GetBackground()->SetColor(Color(0.5, 0.5, 0.5, 1.0f));
//	spriteEditor->
}

void ResourcePackerScreen::UnloadResources()
{
	SafeRelease(resourceTree);
	SafeRelease(spriteEditor);
	
	RemoveAllControls();
}

String ResourcePackerScreen::GetProcessFolderName()
{
	return "$process";
}

void ResourcePackerScreen::PackResources()
{
	Logger::Debug("Input: %s Output: %s Exclude: %s", inputGfxDirectory.c_str(), outputGfxDirectory.c_str(), excludeDirectory.c_str());
	
	isGfxModified = false;

	String path, lastDir;
	FileSystem::SplitPath(inputGfxDirectory, path, lastDir);
	std::transform(lastDir.begin(), lastDir.end(), lastDir.begin(), ::tolower);
	gfxDirName = lastDir;


	String processDirectoryPath = excludeDirectory + String("/") + GetProcessFolderName() + String("/");
	if (FileSystem::Instance()->CreateDirectory(processDirectoryPath, true) == FileSystem::DIRECTORY_CANT_CREATE)
	{
		//Logger::Error("Can't create directory: %s", processDirectoryPath.c_str());
	}


	if (IsMD5ChangedDir(excludeDirectory + String("/") + GetProcessFolderName(), outputGfxDirectory, gfxDirName + ".md5", true))
	{
		if (Core::Instance()->IsConsoleMode())
			printf("[Gfx not available or changed - performing full repack]\n");
		isGfxModified = true;
	
		// Remove whole output directory
		bool result = FileSystem::Instance()->DeleteDirectory(outputGfxDirectory);
		if (result)
		{
			Logger::Debug("Removed output directory: %s", outputGfxDirectory.c_str());
		}
		if (!result && Core::Instance()->IsConsoleMode())
		{
			printf("[ERROR: Can't delete directory %s]\n", outputGfxDirectory.c_str());
		}
	}

	RecursiveTreeWalk(inputGfxDirectory, outputGfxDirectory);

	// Put latest md5 after convertation
	IsMD5ChangedDir(excludeDirectory + String("/") + GetProcessFolderName(), outputGfxDirectory, gfxDirName + ".md5", true);
}


bool ResourcePackerScreen::IsMD5ChangedDir(const String & processDirectoryPath, const String & pathname, const String & name, bool isRecursive)
{
	String pathnameWithoutExtension = FileSystem::ReplaceExtension(name, "");
	String md5FileName = processDirectoryPath + String("/") + pathnameWithoutExtension + ".md5";

	uint8 oldMD5Digest[16];
	uint8 newMD5Digest[16];
	bool isChanged = false;
	File * file = File::Create(md5FileName, File::OPEN | File::READ);
	if (!file)
	{
		isChanged = true;		
	}else
	{
		int32 bytes = file->Read(oldMD5Digest, 16);
		DVASSERT(bytes == 16 && "We should always read 16 bytes from md5 file");
	}
	SafeRelease(file);


	MD5::ForDirectory(pathname, newMD5Digest, isRecursive);

	file = File::Create(md5FileName, File::CREATE | File::WRITE);
	int32 bytes = file->Write(newMD5Digest, 16);
	DVASSERT(bytes == 16 && "16 bytes should be always written for md5 file");
	SafeRelease(file);

	// if already changed return without compare
	if (isChanged)
		return true;

	for (int32 k = 0; k < 16; ++k)
		if (oldMD5Digest[k] != newMD5Digest[k])
			isChanged = true;
	
// 	if (isChanged == true)
// 	{
// 		printf("md5 old:");
// 		for (int32 k = 0; k < 16; ++k)
// 			printf("%x", oldMD5Digest[k]);
// 		printf(" md5 new:");
// 		for (int32 k = 0; k < 16; ++k)
// 			printf("%x", newMD5Digest[k]);
// 		printf("\n");
// 	}
	
	return isChanged;
}


bool ResourcePackerScreen::IsMD5ChangedFile(const String & processDirectoryPath, const String & pathname, const String & psdName)
{
	String pathnameWithoutExtension = FileSystem::ReplaceExtension(psdName, "");
	String md5FileName = processDirectoryPath + String("/") + pathnameWithoutExtension + ".md5";

	uint8 oldMD5Digest[16];
	uint8 newMD5Digest[16];
	bool isChanged = false;
	File * file = File::Create(md5FileName, File::OPEN | File::READ);
	if (!file)
	{
		isChanged = true;		
	}else
	{
		int32 bytes = file->Read(oldMD5Digest, 16);
		DVASSERT(bytes == 16 && "We should always read 16 bytes from md5 file");
	}
	SafeRelease(file);

		
	MD5::ForFile(pathname, newMD5Digest);
	
	file = File::Create(md5FileName, File::CREATE | File::WRITE);
	int32 bytes = file->Write(newMD5Digest, 16);
	DVASSERT(bytes == 16 && "16 bytes should be always written for md5 file");
	SafeRelease(file);

	// file->Write()

	for (int32 k = 0; k < 16; ++k)
		if (oldMD5Digest[k] != newMD5Digest[k])
			isChanged = true;
	
	return isChanged;
}

DefinitionFile * ResourcePackerScreen::ProcessPSD(const String & processDirectoryPath, const String & psdPathname, const String & psdName)
{
	int32 maxTextureSize = 1024;
	if (CommandLineParser::Instance()->IsFlagSet("--tsize2048"))
	{
		maxTextureSize = 2048;
	}
	
	// TODO: Check CRC32
	std::vector<Magick::Image> layers;
	
	String psdNameWithoutExtension = FileSystem::ReplaceExtension(psdName, "");
	
	try 
	{
		Magick::readImages(&layers, psdPathname);
		
		if (layers.size() == 0)
		{
			Logger::Error("Number of layers is too low: %s", psdPathname.c_str());
			return 0;
		}
		
		if (layers.size() == 1)
		{
			layers.push_back(layers[0]);
		}
		
		//Logger::Debug("psd file: %s wext: %s", psdPathname.c_str(), psdNameWithoutExtension.c_str());
		
		int width = (int)layers[0].columns();
		int height = (int)layers[0].rows();
		
		for(int k = 1; k < (int)layers.size(); ++k)
		{
			Magick::Image & currentLayer = layers[k];
			
			
			/* 
			MagickCore::ResetImagePropertyIterator(currentLayer.image());
			const char * property = MagickCore::GetNextImageProperty(currentLayer.image());
			if (property != (const char *) NULL)
			{
				printf("  Properties:\n");
				while (property != (const char *) NULL)
				{
					printf("    %c",*property);
					if (strlen(property) > 1)
						printf("%s: ",property+1);
					if (strlen(property) > 80)
						printf("\n");

					const char * value = MagickCore::GetImageProperty(currentLayer.image(), property);
					if (value != (const char *) NULL)
						printf("%s\n",value);
					property = MagickCore::GetNextImageProperty(currentLayer.image());
				}
			} */
			
			
			
			currentLayer.crop(Magick::Geometry(width,height, 0, 0));
			currentLayer.magick("PNG");
			String outputFile = processDirectoryPath + String("/") + psdNameWithoutExtension;
			outputFile += String(Format("%d.png", k - 1));
			currentLayer.write(outputFile);
		}
		
		
		DefinitionFile * defFile = new DefinitionFile;
		defFile->filename = processDirectoryPath + String("/") + psdNameWithoutExtension + String(".txt");
		// Logger::Debug("filename: %s", defFile->filename.c_str());
		defFile->spriteWidth = width;
		defFile->spriteHeight = height;
		defFile->frameCount = (int)layers.size() -1;
		defFile->frameRects = new Rect2i[defFile->frameCount];
		
		for(int k = 1; k < (int)layers.size(); ++k)
		{
			Magick::Image & currentLayer = layers[k];
			Magick::Geometry bbox = currentLayer.page();
			int xOff = (int)bbox.xOff();
			if (bbox.xNegative())
				xOff = -xOff;
			int yOff = (int)bbox.yOff();
			if (bbox.yNegative())
				yOff = -yOff;
			
			defFile->frameRects[k - 1] = Rect2i(xOff, yOff, (int32)bbox.width(), (int32)bbox.height());
			
			//printf("Percent: %d Aspect: %d Greater: %d Less: %d\n", (int)bbox.percent(), (int)bbox.aspect(), (int)bbox.greater(), (int)bbox.less());
			
			if ((defFile->frameRects[k - 1].dx >= maxTextureSize) || (defFile->frameRects[k - 1].dy >= maxTextureSize))
			{
				
				printf("* WARNING * - frame of %s layer %d is bigger than maxTextureSize(%d) layer exportSize (%d x %d) FORCE REDUCE TO (%d x %d). Bewarned!!! Results not guaranteed!!!\n", psdName.c_str(), k - 1, maxTextureSize
					   , defFile->frameRects[k - 1].dx, defFile->frameRects[k - 1].dy, width, height);
				defFile->frameRects[k - 1].dx = width;
				defFile->frameRects[k - 1].dy = height;
			}
				
			
			
			if (CommandLineParser::Instance()->IsFlagSet("--add0pixel"))
			{
				
			}else if (CommandLineParser::Instance()->IsFlagSet("--add1pixel"))
			{
				defFile->frameRects[k - 1].dx++;
				defFile->frameRects[k - 1].dy++;
			}
			else if (CommandLineParser::Instance()->IsFlagSet("--add2pixel"))
			{
				defFile->frameRects[k - 1].dx+=2;
				defFile->frameRects[k - 1].dy+=2;
			}
			else if (CommandLineParser::Instance()->IsFlagSet("--add4pixel"))
			{
				defFile->frameRects[k - 1].dx+=4;
				defFile->frameRects[k - 1].dy+=4;
			}else 
			{
				defFile->frameRects[k - 1].dx++;
				defFile->frameRects[k - 1].dy++;	
			}
		}
		
		return defFile;
	}
	catch( Magick::Exception &error_ )
    {
		std::cout << "Caught exception: " << error_.what() << " file: "<< psdPathname << std::endl;
		return 0;
    }
	return 0;
}

void ResourcePackerScreen::ProcessFlags(const String & flagsPathname)
{
	File * file = File::Create(flagsPathname.c_str(), File::READ | File::OPEN);
	if (!file)
	{
		Logger::Error("Failed to open file: %s", flagsPathname.c_str());
	}
	char flagsTmpBuffer[4096];
	int flagsSize = 0;
	while(!file->IsEof())
	{
		char c;
		int32 readSize = file->Read(&c, 1);
		if (readSize == 1)
		{
			flagsTmpBuffer[flagsSize++] = c;
		}	
	}
	flagsTmpBuffer[flagsSize++] = 0;
	
	currentFlags = flagsTmpBuffer;
	String flags = flagsTmpBuffer;
	
	const String & delims=" ";
	
	// Skip delims at beginning, find start of first token
	String::size_type lastPos = flags.find_first_not_of(delims, 0);
	// Find next delimiter @ end of token
	String::size_type pos     = flags.find_first_of(delims, lastPos);
	// output vector
	Vector<String> tokens;
	
	while (String::npos != pos || String::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(flags.substr(lastPos, pos - lastPos));
		// Skip delims.  Note the "not_of". this is beginning of token
		lastPos = flags.find_first_not_of(delims, pos);
		// Find next delimiter at end of token.
		pos     = flags.find_first_of(delims, lastPos);
	}
	
	if (CommandLineParser::Instance()->GetVerbose())
		for (int k = 0; k < (int) tokens.size(); ++k)
		{
			Logger::Debug("Token: %s", tokens[k].c_str());
		}

	if (Core::Instance()->IsConsoleMode())
	{
		for (int k = 0; k < (int) tokens.size(); ++k)
		{
			String sub = tokens[k].substr(0, 2);
			if (sub != "--")
				printf("\n[WARNING: flag %s incorrect]\n", tokens[k].c_str());
		}
	}
	
	CommandLineParser::Instance()->SetFlags(tokens);
	
	SafeRelease(file);
}


void ResourcePackerScreen::RecursiveTreeWalk(const String & inputPath, const String & outputPath)
{
	uint64 packTime = SystemTimer::Instance()->AbsoluteMS();

	FileList * fileList = new FileList(inputPath);

	/* New $process folder structure */
	
	String dataSourceRelativePath = inputPath;
	StringReplace(dataSourceRelativePath, excludeDirectory, std::string(""));
	// printf("%s\n", dataSourceRelativePath.c_str());

	String processDirectoryPath = excludeDirectory + String("/") + GetProcessFolderName() + String("/") + dataSourceRelativePath;
	if (FileSystem::Instance()->CreateDirectory(processDirectoryPath, true) == FileSystem::DIRECTORY_CANT_CREATE)
	{
		//Logger::Error("Can't create directory: %s", processDirectoryPath.c_str());
	}

	//String outputPath = outputPath;
	if (FileSystem::Instance()->CreateDirectory(outputPath) == FileSystem::DIRECTORY_CANT_CREATE)
	{
		//Logger::Error("Can't create directory: %s", outputPath.c_str());
	}
	
	CommandLineParser::Instance()->ClearFlags();
	std::list<DefinitionFile *> definitionFileList;

	// Find flags and setup them
	for (int fi = 0; fi < fileList->GetCount(); ++fi)
	{
		if (!fileList->IsDirectory(fi))
		{
			if (fileList->GetFilename(fi) == "flags.txt")
			{
				String fullname = inputPath + String("/") + fileList->GetFilename(fi);
				ProcessFlags(fullname);
				break;
			}
		}
	}
	
	bool modified = isGfxModified;
	// Process all psd / png files

	if (IsMD5ChangedDir(processDirectoryPath, inputPath, "dir.md5", false))
	{
		modified = true;
		//if (Core::Instance()->IsConsoleMode())
		//	printf("[Directory changed - rebuild: %s]\n", inputGfxDirectory.c_str());
	}

	if (modified)
	{
		FileSystem::Instance()->DeleteDirectoryFiles(outputPath, false);
		
		for (int fi = 0; fi < fileList->GetCount(); ++fi)
		{
			if (!fileList->IsDirectory(fi))
			{
				String fullname = inputPath + String("/") + fileList->GetFilename(fi);
				if (FileSystem::GetExtension(fullname) == ".psd")
				{
					DefinitionFile * defFile = ProcessPSD(processDirectoryPath, fullname, fileList->GetFilename(fi));
					definitionFileList.push_back(defFile);
				}else if (FileSystem::GetExtension(fullname) == ".pngdef")
				{
					DefinitionFile * defFile = new DefinitionFile();
					if (defFile->LoadPNGDef(fullname, processDirectoryPath))
					{
						definitionFileList.push_back(defFile);
					}else {
						SafeDelete(defFile);
					}
				}
			}
		}

		// 
		if (definitionFileList.size() > 0 && modified)
		{
			TexturePacker packer;
			String outputPathWithSlash = outputPath + String("/");
				
			bool isSplit = false;
			if (CommandLineParser::Instance()->IsFlagSet("--split"))
				isSplit = true;

			if (!isSplit)
			{
				packer.PackToTextures(excludeDirectory.c_str(), outputPathWithSlash.c_str(), definitionFileList);
			}else
			{
				packer.PackToTexturesSeparate(excludeDirectory.c_str(), outputPathWithSlash.c_str(), definitionFileList);
			}
		}
	}	

	packTime = SystemTimer::Instance()->AbsoluteMS() - packTime;

	if (Core::Instance()->IsConsoleMode())
	{
		if (CommandLineParser::Instance()->IsExtendedOutput())
		{
			printf("[%d files packed with flags: %s]\n", (int)definitionFileList.size(), currentFlags.c_str());
		}
	
		String result = "[unchanged]";
		if (modified)
			result = "[REPACKED]";

		printf("[%s - %.2lf secs] - %s\n", inputPath.c_str(), (float64)packTime / 1000.0f, result.c_str());
	}

	
	for (std::list<DefinitionFile*>::iterator it = definitionFileList.begin(); it != definitionFileList.end(); ++it)
	{
		DefinitionFile * file = *it;
		SafeDelete(file);
	}
	definitionFileList.clear();
	
	for (int fi = 0; fi < fileList->GetCount(); ++fi)
	{
		if (fileList->IsDirectory(fi))
		{
			String filename = fileList->GetFilename(fi);
			if ((filename != ".") && (filename != "..") && (filename != "$process") && (filename != ".svn"))
			{
				if ((filename.size() > 0) && (filename[0] != '.'))
					RecursiveTreeWalk(inputPath + String("/") + fileList->GetFilename(fi),
									  outputPath + String("/") + fileList->GetFilename(fi));
			}
		}
	}
	
	SafeRelease(fileList);
}

// Sprite Editor Logic
void ResourcePackerScreen::OnCellSelected(UIFileTree * tree, UIFileTreeCell *selectedCell)
{
	UITreeItemInfo * itemInfo = selectedCell->GetItemInfo();
	String extension = FileSystem::GetExtension(itemInfo->GetName());
	if (extension == ".txt")
	{
		String spriteNameWithoutExtension = FileSystem::ReplaceExtension(itemInfo->GetPathname(), "");
		OpenSpriteEditor(spriteNameWithoutExtension);
	}
}

void ResourcePackerScreen::OpenSpriteEditor(const String & spriteName)
{
	if (spriteEditor->GetParent() == 0)
	{
		AddControl(spriteEditor);
	}
	spriteEditor->SetPreviewSprite(spriteName);
}

void ResourcePackerScreen::WillAppear()
{
}

void ResourcePackerScreen::WillDisappear()
{
	
}

void ResourcePackerScreen::Input(UIEvent * touch)
{
	
}

void ResourcePackerScreen::Draw(const UIGeometricData &geometricData)
{

}
