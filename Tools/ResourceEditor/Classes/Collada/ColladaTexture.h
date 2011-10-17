#ifndef __COLLADALOADER_COLLADATEXTURE_H__
#define __COLLADALOADER_COLLADATEXTURE_H__

#include "ColladaIncludes.h"

namespace DAVA
{

class ColladaTexture
{
public:
	ColladaTexture(FCDImage * image);
	~ColladaTexture();
	
	bool PreLoad();
	void ProcessDevilImage();

	GLuint	GetTextureId() { return textureId; };

	FCDImage  * image;

	GLuint		textureId;
	bool		hasOpacity;
	
	fstring texturePathName;
};

};

#endif // __COLLADALOADER_TEXTURE_H__


