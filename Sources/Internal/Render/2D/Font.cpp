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
#include "Render/2D/Font.h"
#include "Core/Core.h"
namespace DAVA
{
	
	
int32 Font::globalFontDPI = 72;

void Font::SetDPI(int32 dpi)
{
	globalFontDPI = dpi;
}

int32 Font::GetDPI()
{
	return globalFontDPI;
}
	
Font::Font()
:	size(14.0f)
,	color(1.0f, 0.0f, 1.0f, 1.0f)
,	verticalSpacing(0)
{
}

Font::~Font()
{
	
}


void Font::SetColor(float32 r, float32 g, float32 b, float32 a)
{
	color.r = r; 
	color.g = g; 
	color.b = b; 
	color.a = a;
}

void Font::SetColor(const Color & _color)
{
	color = _color;
}

const Color & Font::GetColor() const
{
	return color;
}

bool Font::IsEqual(Font *font)
{
	if (fontType != font->fontType) 
	{
		return false;
	}
	if (size != size || color != font->color || verticalSpacing != font->verticalSpacing) 
	{
		return false;
	}
	
	return true;
}

void Font::SetSize(float32 _size)
{
	size = _size;
}

float32	Font::GetSize()
{
	return size;
}

void Font::SetVerticalSpacing(int32 _verticalSpacing)
{
	verticalSpacing = _verticalSpacing;
}

int32 Font::GetVerticalSpacing()
{
	return verticalSpacing;
}

void Font::SplitTextToStrings(const WideString & text, const Vector2 & targetRectSize, Vector<WideString> & resultVector)
{
	int32 targetWidth = (int32)(targetRectSize.dx * Core::GetVirtualToPhysicalFactor());

	enum
	{
		SKIP = 0,
		GOODCHAR,	// all characters we like (symbols, special chars, except \n and space
		NEXTLINE, // state after symbol '\'
		FINISH,	// process last line
		EXIT,
	};
	
	resultVector.clear();
	//int textLength = text.length();
	int state = SKIP;
	int lastWordStart = 0;
	int lastWordEnd = 0;
	int currentLineStart = -1;
	int currentLineEnd = 0;
	int totalSize = (int)text.length();
	
	Vector<int32> sizes;
	GetStringSize(text, &sizes);
	
	for(int pos = 0; state != EXIT; pos++)
	{
		wchar_t t = 0;
		if(pos < totalSize)
		{
			t = text[pos];
		}
		switch (state) 
		{
			case SKIP:
				if (t == 0){ state = FINISH; break; } // if end of string process FINISH state and exit
				else if (t == ' ')break; // if space continue with the same state
				else if(t == '\n')
				{
					// this block is copied from case NEXTLINE: if(t == 'n')
					// unlike in NEXTLINE where we ignore 2 symbols, here we ignore only one
					// so last position is pos instead of (pos-1)
					if (currentLineStart != -1) // if we already have something in current line we add to result
					{
						//Logger::Debug("before=%d %d", currentLineStart, pos - 1);
						WideString currentLineWithoutLastWord = text.substr(currentLineStart, pos - currentLineStart);
						//Logger::Debug(L"after=%S", currentLineWithoutLastWord.c_str());
						resultVector.push_back(currentLineWithoutLastWord);
						
						currentLineStart = -1;	// start seach of characters for the new line
					}else
					{
						resultVector.push_back(L""); // here we add empty line if there was no characters in current line
					}
					state = SKIP; //always switch to SKIP because we do not know here what will be next
					break;
				}
				else if (t == '\\'){ state = NEXTLINE; break; } // if find \ go to NEXTLINE state to check if we will get n char
				else // everything else is good characters
				{
					state = GOODCHAR;
					lastWordStart = pos;
					lastWordEnd = pos;
					if (currentLineStart == -1)currentLineStart = pos;
				}
				break;
			case GOODCHAR: 
				if ((t == ' ') || (t == '\\') || t == 0) // if we've found any possible separator process current line 
				{ 
					lastWordEnd = pos;
					
					//					WideString currentLine = text.substr(currentLineStart, lastWordEnd - currentLineStart);
					//					Size2i currentLineSize = GetStringSize(currentLine);
					int currentLineDx = 0;
					for (int i = currentLineStart; i < lastWordEnd ; i++) 
					{
						currentLineDx += sizes[i];
					}
					if (currentLineDx < targetWidth) // if current line size < rect size set line end to current word end
					{
						currentLineEnd = lastWordEnd;
					}else // here we add current line to results because current word is too big for current line
					{
						//Logger::Debug("before=%d %d", currentLineStart, currentLineEnd);
						WideString currentLineWithoutLastWord = text.substr(currentLineStart, currentLineEnd - currentLineStart);
						//Logger::Debug(L"after=%S", currentLineWithoutLastWord.c_str());
						resultVector.push_back(currentLineWithoutLastWord);
						currentLineStart = lastWordStart;
						//fix: 
						// there can be case when last word on current line with one more word (lastWordEnd = pos;) will be wider than targetRect.dx
						// in this case currentLineEnd could have been less than currentLineStart
						currentLineEnd = lastWordEnd;   
					}
				}
				if (t == ' ')state = SKIP; // if cur char is space go to ski[
				else if(t == '\n')
				{
					// this block is copied from case NEXTLINE: if(t == 'n')
					// unlike in NEXTLINE where we ignore 2 symbols, here we ignore only one
					// so last position is pos instead of (pos-1)
					if (currentLineStart != -1) // if we already have something in current line we add to result
					{
						//Logger::Debug("before=%d %d", currentLineStart, pos - 1);
						WideString currentLineWithoutLastWord = text.substr(currentLineStart, pos - currentLineStart);
						//Logger::Debug(L"after=%S", currentLineWithoutLastWord.c_str());
						resultVector.push_back(currentLineWithoutLastWord);
						
						currentLineStart = -1;	// start seach of characters for the new line
					}else
					{
						resultVector.push_back(L""); // here we add empty line if there was no characters in current line
					}
					state = SKIP; //always switch to SKIP because we do not know here what will be next
					break;
				}
				else if (t == '\\') state = NEXTLINE; // if \ check NEXTLINE 
				else if (t == 0) state = FINISH; 
				
				break;
			case NEXTLINE:
				if (t == 0){ state = FINISH; break; } // if found 0 go to finish
				else if (t == 'n') // we found n so this mean we should go to next line
				{	
					if (currentLineStart != -1) // if we already have something in current line we add to result
					{
						//Logger::Debug("before=%d %d", currentLineStart, pos - 1);
						WideString currentLineWithoutLastWord = text.substr(currentLineStart, (pos - 1) - currentLineStart);
						//Logger::Debug(L"after=%S", currentLineWithoutLastWord.c_str());
						resultVector.push_back(currentLineWithoutLastWord);
						
						currentLineStart = -1;	// start seach of characters for the new line
					}else
					{
						resultVector.push_back(L""); // here we add empty line if there was no characters in current line
					}
					state = SKIP; //always switch to SKIP because we do not know here what will be next
					break;
				}else if (t == ' ')
				{	
					state = SKIP; // found space after \ this mean that we go to skip
					break;
				}
				else // found other char so we go to goodchar because we haven't broken the line yet
				{
					state = GOODCHAR;
					break;
				};
				break;
			case FINISH:
				if (currentLineStart != -1) // we check if we have something left in currentline and add this line to results
				{
					//Logger::Debug("ending=%d %d", currentLineStart, currentLineEnd);
					WideString currentLine = text.substr(currentLineStart, currentLineEnd - currentLineStart);
					//Logger::Debug(L"after=%S", currentLine.c_str());
					resultVector.push_back(currentLine);
				}
				state = EXIT; // always exit from here
				break;
		};
	};
}

};