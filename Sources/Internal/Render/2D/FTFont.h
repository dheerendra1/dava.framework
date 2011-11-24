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
        * Created by Ivan Petrochenko
=====================================================================================*/
#ifndef __DAVAENGINE_FTFONT_H__
#define __DAVAENGINE_FTFONT_H__

#include "Base/BaseObject.h"
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/2D/Font.h"
#include "Platform/Mutex.h"

struct FT_FaceRec_;
typedef struct FT_FaceRec_*  FT_Face;

struct FT_GlyphRec_;
typedef struct FT_GlyphRec_*  FT_Glyph;

namespace DAVA
{	
	
class FontManager;
class FTInternalFont;

/** 
	\ingroup fonts
	\brief Freetype-based font implementation.
 
	Class is a wrapper to freetype2 library.
 */
class FTFont : public Font
{	
public:
	
	/**
		\brief Factory method.
		\param[in] path - path to freetype-supported file (.ttf, .otf)
		\returns constructed font
	*/
	static		FTFont * Create(const String& path);
	
	virtual		~FTFont();

	/**
		\brief Clone font.
	*/
	FTFont *	Clone();

	/**
		\brief Tests if two fonts are the same.
	*/
	virtual bool IsEqual(Font *font);

	/**
		\brief Get string size(rect).
		\param[in] str - processed string
		\param[in, out] charSizes - if present(not NULL), will contain widths of every symbol in str 
		\returns bounding rect for string in pixels
	*/
	virtual Size2i		GetStringSize(const WideString & str, Vector<int32> *charSizes = NULL);

	/**
		\brief Get height of highest symbol in font.
		\returns height in pixels
	*/
	virtual uint32		GetFontHeight();
	
	/**
		\brief Checks if symbol is present in font.
		\param[in] ch - tested symbol
		\returns true if symbol is available, false otherwise
	*/
	virtual bool		IsCharAvaliable(char16 ch);

	/**
		\brief Draw string to memory buffer
		\param[in, out] buffer - destination buffer
		\param[in] bufWidth - buffer width in pixels
		\param[in] bufHeight - buffer height in pixels
		\param[in] offsetX - starting X offset
		\param[in] offsetY - starting Y offset
		\param[in] justifyWidth - TODO
		\param[in] spaceAddon - TODO
		\param[in] str - string to draw
		\param[in] contentScaleIncluded - TODO
		\returns bounding rect for string in pixels
	*/
	virtual Size2i DrawStringToBuffer(void * buffer, int32 bufWidth, int32 bufHeight, int32 offsetX, int32 offsetY, int32 justifyWidth, int32 spaceAddon, const WideString & str, bool contentScaleIncluded = false);

	virtual bool IsTextSupportsSoftwareRendering() { return true; };

private:
	FTFont(FTInternalFont* internalFont);
	FTInternalFont	* internalFont;
};


	
};

#endif  //__DAVAENGINE_FTFONT_H__