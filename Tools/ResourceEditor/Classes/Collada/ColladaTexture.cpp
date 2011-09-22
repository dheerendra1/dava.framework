#include "stdafx.h"
#include <wchar.h>
#include "ColladaTexture.h"

///*
// INCLUDE DevIL
// */
//#ifdef _UNICODE
//#undef _UNICODE
//#undef UNICODE
//#define UNICODE_DISABLED
//#endif
//
//#include <IL/il.h>
//#include <IL/ilu.h>
//
//#ifdef UNICODE_DISABLED
//#define _UNICODE
//#endif

namespace DAVA
{

ColladaTexture::ColladaTexture(FCDImage * _image)
{
	image = _image;

//	// Create an image container in DevIL.
//	ILuint imageId;
//
//	ilGenImages(1, &imageId);
//	ilBindImage(imageId);
//
//	// do it square
//	// reduce it if it is necessaryt
//	// test this
//	//GL_MAX_TEXTURE_SIZE.
//
//	// initializate some variables
//	hasAlphaChannel = false;
//
//	wchar_t orig[512];
//	const size_t newsize = 256;
//	char nstring[newsize];
//#ifdef _WIN32
//	// convert fstring to char*, amazing code based on http://msdn2.microsoft.com/en-us/library/ms235631(vs.80).aspx
//	size_t convertedChars = 0;
//	
//	swprintf(orig, 512,L"%s", image->GetFilename().c_str() );
//	size_t origsize = wcslen(orig) + 1;
//	
//	wcstombs_s(&convertedChars, nstring, origsize, orig, _TRUNCATE);
//#else
//	swprintf(orig, 512,L"%s", image->GetFilename().c_str() );
//	const wchar_t * origTmp = &orig[0];
//	wcsnrtombs( nstring, &origTmp, 512, 256, NULL);
//#endif
//	texturePathName =  nstring;
//	
//	wprintf(L"* added texture: %s", (wchar_t*)(image->GetFilename().c_str()));
//	printf(" name: %s\n", image->GetDaeId().c_str());
//
//	// Read in the image file into DevIL.
//	if (!ilLoadImage(nstring))	
//	{
//		wchar_t error_message[256];
//		swprintf(error_message, 256, L"This texture could not be opened: %s\n", (wchar_t*)(image->GetFilename().c_str()));
//		wprintf(error_message);
//		
//		ilDeleteImages(1, &imageId);
//		textureId = -1;
//	} else 
//	{
//		// resize if necessary
//		ProcessDevilImage();
//
//
//		// gl work
//		glGenTextures(1, &textureId); /* Texture name generation */
//		GLenum error;
//		if ((error = glGetError()) != GL_NO_ERROR)
//		{
//			printf("OpenGL Error: %x\n", error);
//		}
//
//		glBindTexture(GL_TEXTURE_2D, textureId); /* Binding of texture name */
//
//		// if 4 channels, the last one is the alpha channel
//		if (ilGetInteger(IL_IMAGE_CHANNELS) == 4) 
//			hasAlphaChannel = true;
//
//		// create mipmaps and upload texture to video card memory
//		gluBuild2DMipmaps
//		(
//			GL_TEXTURE_2D,
//			ilGetInteger(IL_IMAGE_CHANNELS),
//			ilGetInteger(IL_IMAGE_WIDTH),
//			ilGetInteger(IL_IMAGE_HEIGHT), 
//			ilGetInteger(IL_IMAGE_FORMAT), 
//			GL_UNSIGNED_BYTE,
//			ilGetData()
//		); 
//
//
//
//
//		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//
//		printf("texture size: %d x %d\n", ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
//
//		// release memory, now opengl have it !
//		ilDeleteImages(1, &imageId); 
//	}
	
}

ColladaTexture::~ColladaTexture()
{
	
}


void ColladaTexture::ProcessDevilImage() 
{


	//int width = ilGetInteger(IL_IMAGE_WIDTH);
	//int height = ilGetInteger(IL_IMAGE_HEIGHT);


	////Fast algorithm to check if a number is a power of two (from wikipedia)
	////x is a power of two \Leftrightarrow (x > 0) and ((x & (x - 1)) == 0)
	//if ((width & (width-1))==0)

	//int larger;

	//// who is bigger
	//if  (width>height)
	//	larger=width;
	//else
	//	larger=height;

	// Retrieve the width/height of the image file.
//	uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
//	uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);
//	uint32 depth = ilGetInteger(IL_IMAGE_DEPTH);
//
//	//	if (width == 0 || height == 0 || depth == 0)
//	//	{
//	//		//std::cout << "Invalid image file: '" << TO_std::string(filename).c_str() << "'." << std::endl;
//	//		ilDeleteImages(1, &imageId);
//	//		return;
//	//	}
//	//
//
//	//Check if the dimensions are power of 2
//	uint16 countW=0, msbW=0, countH=0, msbH=0, countD=0, msbD=0;
//	uint32 mask = 0x00000001;
//	for(uint16 i=0; i<32; i++)
//	{
//		if((width&mask) == mask) { countW++; msbW = i; }
//		if((height&mask) == mask) { countH++; msbH = i; }
//		if((depth&mask) == mask) { countD++; msbD = i; }
//		mask = mask << 1;
//	}
//
//	// Round to the closest power of 2
//	if(countW > 1) 
//	{
//		mask = 1 << (msbW-1);
//		if((width&mask) == mask)
//			width = mask<<2;
//		else
//			width = mask<<1;
//	}
//	if(countH > 1)
//	{
//		mask = 1 << (msbH-1);
//		if((height&mask) == mask)
//			height = mask<<2;
//		else
//			height = mask<<1;
//	}
//	if(countD > 1)
//	{
//		mask = 1 << (msbH-1);
//		if((depth&mask) == mask)
//			depth = mask<<2;
//		else
//			depth = mask<<1;
//	}
//
//	// Resize image
//	if ((countW | countH | countD) > 1)
//	{
//		iluImageParameter(ILU_FILTER, ILU_LINEAR);
//		//if ()
//		iluScale(width, height, depth);
//	}
//
}



bool ColladaTexture::PreLoad()
{
	

	return true;	
}


};

