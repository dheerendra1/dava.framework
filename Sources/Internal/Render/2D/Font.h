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
#ifndef __DAVAENGINE_FONT_H__
#define __DAVAENGINE_FONT_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/Singleton.h"
#include "Base/EventDispatcher.h"

namespace DAVA
{
/**
	\defgroup fonts Font
 */

class Texture;

/** 
	\ingroup fonts
	\brief This class is base class for all fonts.
 
	This class implements all base functions to handle properties of the font.
	All fonts in system should be inherited from this class.
 */
class Font : public BaseObject
{
public:
	enum eFontType 
	{
			TYPE_FT = 0	//!< freetype-based
		,	TYPE_GRAPHICAL //!< sprite-based
	};
	
	Font();
	virtual ~Font();

	/**
		\brief Set global DPI(dots per inch).
		Default value is 72.
		\param[in] dpi DPI value
	*/
	static void SetDPI(int32 dpi);


	/**
		\brief Get global DPI.
	*/
	static int32 GetDPI();
	
	/**
		\brief Set color.
	*/
	virtual void SetColor(float32 r, float32 g, float32 b, float32 a);

	/**
		\brief Set color.
	*/
	virtual void SetColor(const Color & color);

	/**
		\brief Get color.
	*/
	virtual const Color & GetColor() const;
	
	/**
		\brief Set size.
		\param[in] size in points
	*/
	virtual void SetSize(float32 size);

	/**
		\brief Get size.
		\returns size in pixels
	*/
	virtual float32	GetSize();
	
	/**
	 \brief Set vertical spacing.
	 Spacing value is added to vertical range between lines in multiline text.
	 \param[in] verticalSpacing value in pixels
	 */
	virtual void SetVerticalSpacing(int32 verticalSpacing);


	/**
	 \brief Get vertical spacing 
	 \returns vertical spacing value in pixels
	 */
	virtual int32 GetVerticalSpacing();
	
	/**
		\brief Split string into substrings.
		If one word(letters without separators) is longer than targetRectSize.dx, word will not be splitted.
		Separator symbols:
		1. '\n', "\n"(two symbols) - forced split.
		3. ' '(space) - soft split. If substring is longer than targetRectSize.dx, it will be limited to the last found space symbol. 
		\param[in] text - string to be splitted
		\param[in] targetRectSize - targetRectSize.dx sets desirable maximum substring width
		\param[in, out] resultVector - contains resulting substrings
	*/
	void SplitTextToStrings(const WideString & text, const Vector2 & targetRectSize, Vector<WideString> & resultVector);

	/**
		\brief Get string size(rect).
		\param[in] str - processed string
		\param[in, out] charSizes - if present(not NULL), will contain widths of every symbol in str 
		\returns bounding rect for string in pixels
	*/
	virtual Size2i GetStringSize(const WideString & str, Vector<int32> *charSizes = 0) = 0;

	/**
		\brief Checks if symbol is present in font.
		\param[in] ch - tested symbol
		\returns true if symbol is available, false otherwise
	*/
	virtual bool IsCharAvaliable(char16 ch) = 0;

	/**
		\brief Get height of highest symbol in font.
		\returns height in pixels
	*/
	virtual uint32 GetFontHeight() = 0;

	/**
		\brief Clone font.
	*/
	virtual Font * Clone() = 0;

	/**
		\brief Tests if two fonts are the same.
	*/
	virtual bool IsEqual(Font *font);

	/**
		\brief Draw string to screen.
		\param[in] offsetX - starting X offset
		\param[in] offsetY - starting Y offset
		\param[in] str - string to draw
		\param[in] justifyWidth - reserved
		\returns bounding rect for string in pixels
	*/
	virtual Size2i DrawString(float32 offsetX, float32 offsetY, const WideString & str, int32 justifyWidth = 0) { return Size2i(0, 0);};

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
	virtual Size2i DrawStringToBuffer(void * buffer, int32 bufWidth, int32 bufHeight, int32 offsetX, int32 offsetY, int32 justifyWidth, int32 spaceAddon, const WideString & str, bool contentScaleIncluded = false) { return  Size2i(0, 0); };

	//TODO: get rid of this
	virtual bool IsTextSupportsSoftwareRendering() { return false; };
	virtual bool IsTextSupportsHardwareRendering() { return false; };

protected:
	static int32 globalFontDPI;
	
	float32	size;
	Color color;
	int32 verticalSpacing;
	
	eFontType fontType;
};
		
};

#endif // __DAVAENGINE_FONT_H__
