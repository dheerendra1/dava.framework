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
#include <iostream>


#include "TexturePacker.h"
#include "PngImage.h"
#include "CommandLineParser.h"

//using namespace DAVA;


const int CONVERSION_TYPE = 1;
const int PACKED_RESOURCES_BASE_FOLDER = 2;
const int CONVERTED_RESOURCES_FOLDER = 3;
const int UNPACKED_RESOURCE_BASE_FOLDER = 4;



int main (int xargc, char * xargv[]) 
{
	if (xargc <= 4) 
	{
		printf("texturepack [conv_type] [packed_resources_base_folder] [conv_res_folder] [unpack_res_base_folder]\n");
		printf("Flags:\n");
		printf("\t--pvr - pack textures in this folder to pvr format\n");
		printf("\t--tsize2048 - set max texture size to 2048\n");
		printf("\t--add0pixel - set 0 pixels default space between sprite frames\n");
		printf("\t--add1pixel - set 1 pixels default space between sprite frames\n");
		printf("\t--add2pixel - set 2 pixels default space between sprite frames\n");
		printf("\t--add3pixel - set 3 pixels default space between sprite frames\n");
		printf("\tdefault value is 1\n");
		printf("\t--debug - draw debug rects around frames\n");
		printf("\t--split - every PSD in directory will be packed to own texture\n");
		return 0;
	}
	
	CommandLineParser * commandLine = new CommandLineParser(xargc, xargv);
 	

	// insert code here...
	std::string convType = commandLine->GetParam(CONVERSION_TYPE);
	std::string logPath = commandLine->GetParam(UNPACKED_RESOURCE_BASE_FOLDER) + std::string("directory_pack_log.txt");
	
	FILE * stream; 
	if ((stream = freopen(logPath.c_str(), "w", stdout)) == NULL)
	{
		printf("Problem with setting log directory: %s\n", logPath.c_str());
		exit(-1);
	}
	
    std::cout << "* Texture packer launched. Argument count: " << xargc << std::endl;
	for (int k = 0; k < xargc; ++k)
		printf("%s ", xargv[k]);
	printf("\n");
	
	std::cout << "Version: 1.983" << std::endl;
	
	std::list<DefinitionFile*> defsList;
	printf("* Definition file count:%d\n", commandLine->GetParamCount() - 5);
	
	for (int ai = 5; ai < commandLine->GetParamCount(); ++ai)
	{
		String defFilePath = commandLine->GetParam(ai);
		String ext = CommandLineParser::GetExtension(defFilePath);
				
		printf("* Processing file: %s %s\n", defFilePath.c_str(), ext.c_str());

		DefinitionFile *file = new DefinitionFile;
		if (ext == ".txt")
		{		
			if (!file->Load(defFilePath))
			{
				return -1;
			}
		}else if (ext == ".pngdef")
		{
			if (!file->LoadPNGDef(defFilePath))
			{
				return -1;
			}
		}

		defsList.push_back(file);
	}
	bool isSplit = false;
	
	if (commandLine->IsFlagSet("-split"))
		isSplit = true;
	else if (commandLine->IsFlagSet("--split"))
		isSplit = true;
	
	if (!isSplit)
	{	
		TexturePacker packer;
		packer.PackToTextures(commandLine->GetParam(PACKED_RESOURCES_BASE_FOLDER).c_str(), commandLine->GetParam(CONVERTED_RESOURCES_FOLDER).c_str(), defsList);
	}else
	{
		TexturePacker packer;
		packer.PackToTexturesSeparate(commandLine->GetParam(PACKED_RESOURCES_BASE_FOLDER).c_str(), commandLine->GetParam(CONVERTED_RESOURCES_FOLDER).c_str(), defsList);
	}
	//stream = freopen("CON", "w", stdout);
	
    return 0;
}
