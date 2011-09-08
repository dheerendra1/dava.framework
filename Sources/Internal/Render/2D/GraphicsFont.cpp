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
#include "Base/BaseTypes.h"
#include "Render/2D/GraphicsFont.h"
#include "Render/RenderManager.h"
#include "FileSystem/File.h"
#include "Debug/DVAssert.h"

namespace DAVA
{
	
GraphicsFontDefinition::GraphicsFontDefinition()
{
	characterTable = 0;
	characterPreShift = 0;
	characterWidthTable = 0;
	kerningBaseShift = 0;
	kerningTable = 0;
	
}

GraphicsFontDefinition::~GraphicsFontDefinition()
{
	SafeDeleteArray(characterTable);
	SafeDeleteArray(characterPreShift);
	SafeDeleteArray(characterWidthTable);
	SafeDeleteArray(kerningBaseShift);
	SafeDeleteArray(kerningTable);
}
	
GraphicsFont::GraphicsFont()
	:fontSprite(0)
{
	fontType = FONT_TYPE_GRAPHICAL;
	fdef = 0;
	fontScaleCoeff = 1.0f;
}

GraphicsFont::~GraphicsFont()
{
	SafeRelease(fdef);
	SafeRelease(fontSprite);
}	
	
Font * GraphicsFont::Clone()
{
	GraphicsFont * cloneFont = new GraphicsFont();

	cloneFont->fdef = SafeRetain(this->fdef);	
	cloneFont->fontSprite = SafeRetain(this->fontSprite);

	cloneFont->SetColor(this->GetColor());
	cloneFont->SetShadowColor(this->GetShadowColor());
	cloneFont->SetShadowOffset(this->GetShadowOffset());
	cloneFont->SetVerticalSpacing(this->GetVerticalSpacing());
	cloneFont->SetSize(this->GetSize());
	
	return cloneFont;
}

Size2i GraphicsFont::GetStringSize(const WideString & string, Vector<int32> *charSizes)
{
	uint32 length = (uint32)string.length();
	uint16 prevChIndex = GraphicsFontDefinition::INVALID_CHARACTER_INDEX;
	Sprite::DrawState state;
	
	float32 currentX = 0;
	float32 prevX = 0;
	float32 sizeFix = 0.0f;
	for (uint32 indexInString = 0; indexInString < length; ++indexInString)
	{
		char16 c = string[indexInString];
		uint16 chIndex = fdef->CharacterToIndex(c);
		
		
		if (indexInString == 0)
		{
			DVASSERT(chIndex < fdef->tableLenght);
			sizeFix = fontSprite->GetRectOffsetValueForFrame(chIndex, Sprite::X_OFFSET_TO_ACTIVE) * fontScaleCoeff;
			currentX -= sizeFix;
		}
		
		if (chIndex == GraphicsFontDefinition::INVALID_CHARACTER_INDEX)
		{
			Logger::Debug("*** Error: can't find character %c in font", c);
			if (charSizes)charSizes->push_back(0); // push zero size if character is not available
			continue;
		}
		
		if (prevChIndex != GraphicsFontDefinition::INVALID_CHARACTER_INDEX)
		{
			//Logger::Debug("kern: %c-%c = %f", string[indexInString - 1], c,  GetDistanceFromAtoB(prevChIndex, chIndex));
			DVASSERT(chIndex < fdef->tableLenght);
			currentX += GetDistanceFromAtoB(prevChIndex, chIndex);
		}
			
		currentX += fdef->characterWidthTable[chIndex] * fontScaleCoeff;
//		if (c == ' ')currentX += fdef->characterWidthTable[chIndex] * fontScaleCoeff;
//		else currentX += fontSprite->GetRectOffsetValueForFrame(chIndex, Sprite::ACTIVE_WIDTH) * fontScaleCoeff;
		
		// BORODA: Probably charSizes should be float??? 
		if (charSizes)charSizes->push_back((int32)(currentX + sizeFix - prevX));
		prevX = currentX;
		prevChIndex = chIndex;
	}
	prevX = currentX;
	
	DVASSERT(prevChIndex < fdef->tableLenght);

	currentX -= fdef->characterWidthTable[prevChIndex] * fontScaleCoeff;
	currentX += (fdef->characterPreShift[prevChIndex] + fontSprite->GetRectOffsetValueForFrame(prevChIndex, Sprite::ACTIVE_WIDTH)) * fontScaleCoeff; // characterWidthTable[prevChIndex];

//	float32 lastCharSize = fdef->characterWidthTable[prevChIndex] * fontScaleCoeff;
//	float32 lastCharSize = fontSprite->GetRectOffsetValueForFrame(prevChIndex, Sprite::ACTIVE_WIDTH) * fontScaleCoeff;
//	currentX += lastCharSize; // characterWidthTable[prevChIndex];
	if (charSizes)charSizes->push_back((int32)(currentX + sizeFix - prevX));
	return Size2i((int32)(currentX + sizeFix + 1.5f), GetFontHeight());
}
	
bool GraphicsFont::IsCharAvaliable(char16 ch)
{
	return (fdef->CharacterToIndex(ch) != GraphicsFontDefinition::INVALID_CHARACTER_INDEX);
}

uint32 GraphicsFont::GetFontHeight()
{
	return (uint32)(fdef->fontHeight * fontScaleCoeff);
}
	
int32 GraphicsFont::GetAscender()
{
	return (int32) ((fdef->fontAscent + 0.5f) * fontScaleCoeff);
}
int32 GraphicsFont::GetDescender()
{
	return (int32) ((fdef->fontDescent + 0.5f) * fontScaleCoeff);
}
	
void GraphicsFont::SetSize(float32 _size)
{
	size = _size;
	fontScaleCoeff = size / (fdef->fontAscent + fdef->fontDescent);	
}


bool GraphicsFontDefinition::LoadFontDefinition(const String & fontDefName)
{
	File * file = File::Create(fontDefName, File::OPEN | File::READ);
	if (!file)
	{
		return false;
	}
	
	char header[4];
	DVASSERT(file->Read(header, 4) == 4);
	if ((header[0] != 'F') || (header[1] != 'D') || (header[2] != 'E') || (header[3] != 'F'))
	{
		SafeRelease(file);
		return false;
	}
	uint32 version = 0;
	DVASSERT(file->Read(&version, 4) == 4);
	if (version != 1)
	{
		SafeRelease(file);
		return false;
	}
	
	DVASSERT(file->Read(&fontAscent, 4) == 4);
	DVASSERT(file->Read(&fontDescent, 4) == 4);
	DVASSERT(file->Read(&fontLeading, 4) == 4);
	DVASSERT(file->Read(&fontXHeight, 4) == 4);
	DVASSERT(file->Read(&charLeftRightPadding, 4) == 4);
	DVASSERT(file->Read(&charTopBottomPadding, 4) == 4);	
	
	fontHeight = (uint32)(fontAscent + fontDescent + fontLeading + 0.5f);
	
	DVASSERT(file->Read(&tableLenght, 4) == 4);
	characterTable = new char16[tableLenght];
	characterPreShift = new float32[tableLenght];
	characterWidthTable = new float32[tableLenght];
	kerningBaseShift = new float32[tableLenght];
	kerningTable = new KerningPair*[tableLenght];
	
	for (int32 t = 0; t < tableLenght; ++t)
	{
		// BORODA: THIS IS FIX BECAUSE CHAR16 isn't char16 on MacOS and iPhone
		unsigned short c = 0;
		DVASSERT(file->Read(&c, 2) == 2);
		characterTable[t] = c;
		DVASSERT(file->Read(&characterPreShift[t], 4) == 4);
		DVASSERT(file->Read(&characterWidthTable[t], 4) == 4);
		//Logger::Debug("char: %c idx: %d",  characterTable[t], t);
	}
	
	DVASSERT(file->Read(&defaultShiftValue, 4) == 4);
	
	for (int t = 0; t < tableLenght; ++t)
	{
		DVASSERT(file->Read(&kerningBaseShift[t], 4) == 4);
		//Logger::Debug("base: %c baseshift:%f preshift:%f", characterTable[t], kerningBaseShift[t], characterPreShift[t]);
	}
	
	DVASSERT(file->Read(&kerningPairCount, 4) == 4);
	for (int32 k = 0; k < tableLenght; ++k)
		kerningTable[k] = 0;
	
	for (int32 kp = 0; kp < kerningPairCount; ++kp)
	{
		unsigned short s1short;
		DVASSERT(file->Read(&s1short, 2) == 2); 
		unsigned short s2short;
		DVASSERT(file->Read(&s2short, 2) == 2); 
		float32 shift;
		DVASSERT(file->Read(&shift, 4) == 4); 
		
		KerningPair * p = new KerningPair();
		p->ch1Index = s1short;
		p->ch2Index = s2short;
		p->shift = shift;
		AddKerningPair(p);
		//file->Read(&kerningTable[s1][s2], 4, 1, fontFP);
	}
	
//	for (int32 t = 0; t < tableLenght; ++t)
//	{
//		//Logger::Debug("char check: %c idx: %d",  characterTable[t], t);
//	}
	
	
	SafeRelease(file);
	return true;
}

void GraphicsFontDefinition::AddKerningPair(KerningPair * kpair)
{
	kpair->next = kerningTable[kpair->ch1Index];
	kerningTable[kpair->ch1Index] = kpair;
}
	
GraphicsFontDefinition::KerningPair * GraphicsFontDefinition::FindKerningPair(uint16 ch1Index, uint16 ch2Index)
{
	KerningPair * current = kerningTable[ch1Index];	
	while(current != 0)
	{
		if (current->ch2Index == ch2Index)return current;
		current = current->next;
	}
	return 0;
}

uint16 GraphicsFontDefinition::CharacterToIndex(char16 c)
{
	for (int32 ci = 0; ci < tableLenght; ++ci)
	{
		if (characterTable[ci] == c)
		{
			return (uint16)ci;
		}
	}
	return INVALID_CHARACTER_INDEX;
}
	
GraphicsFont * GraphicsFont::Create(const String & fontDefName, const String & spriteName)
{
	GraphicsFont * font = new GraphicsFont();
	font->fdef = new GraphicsFontDefinition();
	if (!font->fdef->LoadFontDefinition(fontDefName))
	{
		Logger::Error("Failed to create font from definition: %s", fontDefName.c_str());
		SafeRelease(font->fdef);
		SafeRelease(font);
		return 0;
	}
	font->size = font->fdef->fontAscent + font->fdef->fontDescent;
	font->fontSprite = Sprite::Create(spriteName);
	if (!font->fontSprite)
	{
		Logger::Error("Failed to create font because sprite is not available: %s", spriteName.c_str());
		SafeRelease(font);
		return 0;
	}
	return font;
}

bool GraphicsFont::IsTextSupportsHardwareRendering() 
{ 
	return true; 
};

float32 GraphicsFont::GetDistanceFromAtoB(int32 prevChIndex, int32 chIndex)
{
	float32 currentX = 0.0f;
	currentX += fdef->defaultShiftValue;
	currentX += fdef->kerningBaseShift[prevChIndex];
	GraphicsFontDefinition::KerningPair * kpair = fdef->FindKerningPair(prevChIndex, chIndex);
	if (kpair)
		currentX += kpair->shift;
	return currentX * fontScaleCoeff;
}

Size2i GraphicsFont::DrawString(float32 x, float32 y, const WideString & string, int32 justifyWidth)
{
	uint32 length = (uint32)string.length();
    if(length==0) return Size2i();
    
	uint16 prevChIndex = GraphicsFontDefinition::INVALID_CHARACTER_INDEX;
	Sprite::DrawState state;

	state.SetPerPixelAccuracyUsage(true);
	
	float32 currentX = x;
	float32 currentY = y;
	float32 sizeFix = 0.0f;
	//Logger::Debug("%S startX:%f", string.c_str(), currentX);
    RenderManager::Instance()->SetColor(color);
	for (uint32 indexInString = 0; indexInString < length; ++indexInString)
	{
		char16 c = string[indexInString];
		uint16 chIndex = fdef->CharacterToIndex(c);
		
		if (indexInString == 0)
		{
			sizeFix = fontSprite->GetRectOffsetValueForFrame(chIndex, Sprite::X_OFFSET_TO_ACTIVE) * fontScaleCoeff;
			currentX -= sizeFix;
		}
		
		if (chIndex == GraphicsFontDefinition::INVALID_CHARACTER_INDEX)
		{
			Logger::Debug("*** Error: can't find character %c in font", c);
			continue;
		}
		
		if (prevChIndex != GraphicsFontDefinition::INVALID_CHARACTER_INDEX)
		{
			//Logger::Debug("kern: %c-%c = %f", string[indexInString - 1], c,  GetDistanceFromAtoB(prevChIndex, chIndex));
			currentX += GetDistanceFromAtoB(prevChIndex, chIndex);
		}
		
		state.SetFrame(chIndex);
		// draw on baseline Y = currentY - fontSprite->GetHeight() + charTopBottomPadding + fontDescent

		float32 drawX = currentX;
		float32 drawY = currentY - fontSprite->GetHeight() * fontScaleCoeff + (fdef->charTopBottomPadding + fdef->fontDescent + fdef->fontAscent) * fontScaleCoeff;
		
		
		//if (indexInString != 0)
		drawX += fdef->characterPreShift[chIndex] * fontScaleCoeff;
		
		
		state.SetScale(fontScaleCoeff, fontScaleCoeff);
		state.SetPosition(drawX, drawY);
        
		fontSprite->Draw(&state);

//		RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
//		RenderManager::Instance()->DrawRect(Rect(drawX + fontSprite->GetRectOffsetValueForFrame(chIndex, Sprite::X_OFFSET_TO_ACTIVE) * fontScaleCoeff, drawY + fontSprite->GetRectOffsetValueForFrame(chIndex, Sprite::Y_OFFSET_TO_ACTIVE) * fontScaleCoeff, fontSprite->GetRectOffsetValueForFrame(chIndex, Sprite::ACTIVE_WIDTH), fontSprite->GetHeight() * fontScaleCoeff));
//		RenderManager::Instance()->ResetColor();
		
		currentX += fdef->characterWidthTable[chIndex] * fontScaleCoeff;
//		Logger::Debug("%c w:%f pos: %f\n", c, fdef->characterWidthTable[chIndex] * fontScaleCoeff, currentX); 
//		if (c == ' ')currentX += fdef->characterWidthTable[chIndex] * fontScaleCoeff;
//		else currentX += fontSprite->GetRectOffsetValueForFrame(chIndex, Sprite::ACTIVE_WIDTH) * fontScaleCoeff;

		prevChIndex = chIndex;
	}
    RenderManager::Instance()->ResetColor();

	currentX -= fdef->characterWidthTable[prevChIndex] * fontScaleCoeff;
	currentX += (fdef->characterPreShift[prevChIndex] + fontSprite->GetRectOffsetValueForFrame(prevChIndex, Sprite::ACTIVE_WIDTH)) * fontScaleCoeff; // characterWidthTable[prevChIndex];
	
//	Sprite::DrawState drwState;
//	float32 drawX = 100;
//	float32 drawY = 100;
//	drwState.SetFrame(1);
//	drwState.SetPosition(drawX, drawY);
//	fontSprite->Draw(&drwState);
//	
//	RenderManager::Instance()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
//	RenderManager::Instance()->DrawRect(Rect(drawX + fontSprite->GetRectOffsetValueForFrame(1, Sprite::X_OFFSET_TO_ACTIVE) * fontScaleCoeff
//											 , drawY + fontSprite->GetRectOffsetValueForFrame(1, Sprite::Y_OFFSET_TO_ACTIVE) * fontScaleCoeff
//											 , fdef->characterWidthTable[1]
//											 , fontSprite->GetHeight() * fontScaleCoeff));
//	RenderManager::Instance()->ResetColor();
	
	
	return Size2i((int32)(currentX + sizeFix + 1.5f - x), GetFontHeight());
}

};