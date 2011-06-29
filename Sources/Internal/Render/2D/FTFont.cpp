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
#include <ft2build.h>
#include <freetype/ftglyph.h>
//#include "ftglyph.h"
#include FT_FREETYPE_H

#include "Render/2D/FTFont.h"
#include "Render/2D/FontManager.h"
#include "FileSystem/Logger.h"
#include "Utils/Utils.h"
#include "Debug/DVAssert.h"
#include "FileSystem/File.h"
#include "Core/Core.h"

namespace DAVA
{

Map<String,FTInternalFont*> fontMap;

FTFont::FTFont(FTInternalFont* _internalFont)
:	Font(),
	r((uint8)(color.r * 255.0f)),  
	g((uint8)(color.g * 255.0f)), 
	b((uint8)(color.b * 255.0f)), 
	a((uint8)(color.a * 255.0f)), 
	sr((uint8)(shadowColor.r * 255.0f)), 
	sg((uint8)(shadowColor.g * 255.0f)), 
	sb((uint8)(shadowColor.b * 255.0f)), 
	sa((uint8)(shadowColor.a * 255.0f)) 
{
	internalFont = _internalFont;
	internalFont->Retain();
	fontType = FONT_TYPE_FT;
}

FTFont::~FTFont()
{
	SafeRelease(internalFont);
}

FTFont * FTFont::Create(const String& path)
{
	FTInternalFont * iFont = 0;
	Map<String,FTInternalFont*>::iterator it = fontMap.find(path);
	if (it != fontMap.end())
	{
		iFont = it->second;
	}
	
	if(!iFont)
	{//TODO: for now internal fonts is never released, need to be fixed later
		iFont = new FTInternalFont(path);
		fontMap[path] = iFont;
	}
	
	FTFont * font = new FTFont(iFont);
	
	return font;
}
	
FTFont *	FTFont::Clone()
{
	FTFont *retFont = new FTFont(internalFont);
	retFont->size =	size;
	
	retFont->r =	r;
	retFont->g =	g;
	retFont->b =	b;
	retFont->a =	a;

	retFont->sr =	sr;
	retFont->sg =	sg;
	retFont->sb =	sb;
	retFont->sa =	sa;

	retFont->shadowOffset =	shadowOffset;
	retFont->verticalSpacing =	verticalSpacing;

	return retFont;
	
}

bool FTFont::IsEqual(Font *font)
{
	if (!Font::IsEqual(font) || internalFont != ((FTFont*)font)->internalFont)
	{
		return false;
	}
	return true;
}


void FTFont::SetColor(float32 _r, float32 _g, float32 _b, float32 _a)
{
	color.r = _r;
	color.g = _g;
	color.b = _b;
	color.a = _a;
	r = (uint8)(_r*255.0);
	g = (uint8)(_g*255.0);
	b = (uint8)(_b*255.0);
	a = (uint8)(_a*255.0);
}

void FTFont::SetShadowColor(float32 _r, float32 _g, float32 _b, float32 _a)
{
	shadowColor.r = _r;
	shadowColor.g = _g;
	shadowColor.b = _b;
	shadowColor.a = _a;
	sr = (uint8)(_r*255.0);
	sg = (uint8)(_g*255.0);
	sb = (uint8)(_b*255.0);
	sa = (uint8)(_a*255.0);
}

void FTFont::SetColor(const Color & color)
{
	SetColor(color.r, color.g, color.b, color.a);
}

void FTFont::SetShadowColor(const Color & color)
{
	SetShadowColor(color.r, color.g, color.b, color.a);
}

	
Size2i FTFont::DrawStringToBuffer(void * buffer, int32 bufWidth, int32 bufHeight, int32 offsetX, int32 offsetY, int32 justifyWidth, const WideString& str, bool contentScaleIncluded )
{
	return internalFont->DrawString(str, buffer, bufWidth, bufHeight, r, g, b, a, sr, sg, sb, sa, shadowOffset, size, true, offsetX, offsetY, justifyWidth, NULL, contentScaleIncluded );
}

Size2i FTFont::GetStringSize(const WideString& str, Vector<int32> *charSizes)
{
	return internalFont->DrawString(str, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, shadowOffset, size, false, 0, 0, 0, charSizes);
}

uint32 FTFont::GetFontHeight()
{
	return internalFont->GetFontHeight(size);
}
	
int32 FTFont::GetAscender()
{
	return internalFont->GetAscender(size);
}
	
int32 FTFont::GetDescender()
{
	return internalFont->GetDescender(size);
}


bool FTFont::IsCharAvaliable(char16 ch)
{
	return internalFont->IsCharAvaliable(ch);
}

	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
FTInternalFont::FTInternalFont(const String& path)
:	face(0),
	fontPath(path)
{

	File * fp = File::Create(path, File::READ|File::OPEN);
	if (!fp)
	{
		Logger::Error("Failed to open font: %s", path.c_str());
		return;
	}

	memoryFontSize = fp->GetSize();
	memoryFont = new uint8[memoryFontSize];
	fp->Read(memoryFont, memoryFontSize);
	SafeRelease(fp);
	
	FT_Error error = FT_New_Memory_Face(FontManager::Instance()->GetFTLibrary(), memoryFont, memoryFontSize, 0, &face);
	if(error == FT_Err_Unknown_File_Format)
	{
		Logger::Error("FTInternalFont::FTInternalFont FT_Err_Unknown_File_Format");
	}
	else if(error)
	{
		Logger::Error("FTInternalFont::FTInternalFont cannot create font(no file?)");
	}
}
	
FTInternalFont::~FTInternalFont()
{
	FT_Done_Face(face);
	SafeDeleteArray(memoryFont);
}


int32 FTInternalFont::Release()
{
	if(1 == GetRetainCount())
	{
		fontMap.erase(fontPath);
	}
	
	return BaseObject::Release();
}

Mutex FTInternalFont::drawStringMutex;

Size2i FTInternalFont::DrawString(const WideString& str, void * buffer, int32 bufWidth, int32 bufHeight, 
					uint8 r, uint8 g, uint8 b, uint8 a, 
					uint8 sr, uint8 sg, uint8 sb, uint8 sa, 
					const Vector2 & shadowOffset, 
					float32 size, bool realDraw, 
					int32 offsetX, int32 offsetY,
					int32 justifyWidth,
					Vector<int32> *charSizes,
					bool contentScaleIncluded )
{
	drawStringMutex.Lock();

	bool isShadow = (sa != 0);
	
	if (!contentScaleIncluded) 
	{
		bufWidth = (int32)(Core::GetVirtualToPhysicalFactor() * bufWidth);
		bufHeight = (int32)(Core::GetVirtualToPhysicalFactor() * bufHeight);
		offsetY = (int32)(Core::GetVirtualToPhysicalFactor() * offsetY);
		offsetX = (int32)(Core::GetVirtualToPhysicalFactor() * offsetX);
	}

	int32 penX = offsetX;
	int32 penY = offsetY;
	int16 * resultBuf = (int16*)buffer;
	
	
	 
	SetFTCharSize(size);
	
	bool use_kerning = (FT_HAS_KERNING(face) > 0);
	int32 previous = 0;
	
	FT_UInt num_glyphs = 0;
	
	int32 num_chars = (int32)str.length();
	FT_Glyph * glyphs = new FT_Glyph[num_chars];
	FT_Vector * pos = new FT_Vector[num_chars];
	
	//calculate string rect
	FT_BBox  bbox;
	
	
	/* initialize string bbox to "empty" values */
	bbox.xMin = bbox.yMin =  32000;
	bbox.xMax = bbox.yMax = -32000;
	int prevSz = 0;
	int spacesCount = 0;
	
	for(int32 n = 0; n < num_chars; n++)
	{
		/* convert character code to glyph index */
		if (str[n] == L' ') 
		{
			spacesCount++;
		}
		int32 glyph_index = FT_Get_Char_Index(face, str[n]);
		
		/* retrieve kerning distance and move pen position */
		if (use_kerning && previous && glyph_index)
		{
			FT_Vector  delta;
			FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
			penX += delta.x >> 6;
		}
		
		/* store current pen position */
		pos[num_glyphs].x = penX;
		
		
		/* load glyph image into the slot without rendering */
		FT_Error error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
		if ( error )
		{
			if(charSizes)
			{
				(*charSizes).push_back(0);
			}
			continue;  /* ignore errors, jump to next glyph */
		}
		
		/* extract glyph image and store it in our table */
		error = FT_Get_Glyph( face->glyph, &glyphs[num_glyphs] );
		if ( error )
		{
			if(charSizes)
			{
				(*charSizes).push_back(0);
			}
			continue;  /* ignore errors, jump to next glyph */
		}
		
		/* increment pen position */
		//penX += face->glyph->metrics.width >> 6;
		penX += (face->glyph->metrics.horiAdvance + (1<<5)) >> 6;
		//pos[num_glyphs].y = penY - ((face->glyph->metrics.height - face->glyph->metrics.horiBearingY) >>6);
		pos[num_glyphs].y = ((face->glyph->metrics.horiBearingY + (1<<5)) >>6);
		/* record current glyph index */
		previous = glyph_index;
		
		
		
		/* for each glyph image, compute its bounding box, */
		/* translate it, and grow the string bbox          */
		FT_BBox  glyph_bbox;
		
		
		FT_Glyph_Get_CBox(glyphs[num_glyphs], ft_glyph_bbox_pixels, &glyph_bbox );
		
		if(!glyph_bbox.xMin && !glyph_bbox.xMax && !glyph_bbox.yMin && !glyph_bbox.yMax)
		{
			//SPACE symbol
			num_glyphs++;
			if(charSizes)
			{
				(*charSizes).push_back(0);
			}
			continue;
		}
		glyph_bbox.xMin += pos[num_glyphs].x;
		glyph_bbox.xMax += pos[num_glyphs].x;
		
		if(isShadow)
		{
			if(shadowOffset.x > 0) glyph_bbox.xMax += (FT_Pos)shadowOffset.x;
			if(shadowOffset.x < 0) glyph_bbox.xMin += (FT_Pos)shadowOffset.x;
			if(shadowOffset.y > 0) glyph_bbox.yMax += (FT_Pos)shadowOffset.y;
			if(shadowOffset.y < 0) glyph_bbox.yMin += (FT_Pos)shadowOffset.y;
		}
		
		if (glyph_bbox.xMin < bbox.xMin)
			bbox.xMin = glyph_bbox.xMin;
		
		if (glyph_bbox.yMin < bbox.yMin)
			bbox.yMin = glyph_bbox.yMin;
		
		if (glyph_bbox.xMax > bbox.xMax)
			bbox.xMax = glyph_bbox.xMax;
		
		if (glyph_bbox.yMax > bbox.yMax)
			bbox.yMax = glyph_bbox.yMax;
		
		
		if(charSizes)
		{
			int sz = bbox.xMax-bbox.xMin;
			(*charSizes).push_back(sz - prevSz);
			prevSz = sz;
		}
		/* increment number of glyphs */
		num_glyphs++;
	}


	//if(bbox.yMin > 0) bbox.yMin = 0;
	if(bbox.xMin > 0) bbox.xMin = 0;

	/* check that we really grew the string bbox */
	if ( bbox.xMin > bbox.xMax )
	{
		bbox.xMin = 0;
		bbox.yMin = 0;
		bbox.xMax = 0;
		bbox.yMax = 0;
	}
	
	if(realDraw)
	{
		if (justifyWidth > 0 && spacesCount > 0) 
		{
			int oversize = justifyWidth - (bbox.xMax - bbox.xMin);
			int spaceAdd = oversize / spacesCount;
			int totalAdd = 0;
			if (oversize > 0) 
			{
				oversize -= spacesCount * spaceAdd;
				for(int32 n = 0; n < num_chars; n++)
				{
					/* convert character code to glyph index */
					if (str[n] == L' ') 
					{
						if (oversize > 0)
						{
							totalAdd += spaceAdd + 1;
							oversize--;
						}
						else 
						{
							totalAdd += spaceAdd;
						}
						
					}
					pos[n].x = pos[n].x + totalAdd;
				}
			}
		}
		//create bufer
		//uint32 width = 256;//bbox.xMax - bbox.xMin;
		//uint32 height = 256;//bbox.yMax - bbox.yMin;
		//int16 * resultBuf = new int16[bufWidth*bufHeight];
		
		// VB: Please be patient --- please do not clear the buffer here 
		// because it can break multiline text
		// Dizz: I'm patient just as a boa
		
		if(isShadow)
		{
			for (uint32 n = 0; n < num_glyphs; n++)
			{
				FT_Glyph   image;
				FT_Vector  pen;
				
				image = glyphs[n];
				
				pen.x = (FT_Pos)(pos[n].x + shadowOffset.x);
				pen.y = (FT_Pos)(penY - pos[n].y + ((face->size->metrics.ascender+ (1<<5))>>6) + shadowOffset.y);
				
				//DVASSERT(pen.x >= 0 && pen.y >= 0);
				
	//			if(pen.x > bufWidth || pen.y > bufHeight)
	//			{
	//				continue;
	//			}
				
				FT_Error error = FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, 0, 0);
				if ( !error )
				{
					FT_BitmapGlyph  bit = (FT_BitmapGlyph)image;
					FT_Bitmap * bitmap = &bit->bitmap;

					//TODO: optimize
					int32 realH = Min((int32)bitmap->rows, (int32)(bufHeight - pen.y));
					int32 realW = Min((int32)bitmap->width, (int32)(bufWidth - pen.x)); 
					for(int32 h = 0; h < realH; h++)
					{
						for(int32 w = 0; w < realW; w++)
						{
							int lowBoundX = pen.x+w;
							int lowBoundY = pen.y+h;
							
							if(lowBoundX < 0 || lowBoundY < 0)
							{
								continue;
							}
						
							int oldPix = bitmap->buffer[h*bitmap->width+w];
							
							uint8 preAlpha = (oldPix*sa)>>8;
							
							resultBuf[(pen.y+h)*bufWidth + (pen.x+w)] = (
							(((preAlpha*sr)>>12)<<12) |
							(((preAlpha*sg)>>12)<<8) | 
							(((preAlpha*sb)>>12)<<4) | 
							((preAlpha)>>4)); 
						}
					}
					FT_Done_Glyph( image );
				}
			}
			
			
			for (uint32 n = 0; n < num_glyphs; n++)
			{
				FT_Glyph   image;
				FT_Vector  pen;
				
				image = glyphs[n];
				
				pen.x = pos[n].x;
				pen.y = penY - pos[n].y + ((face->size->metrics.ascender+ (1<<5))>>6);
				
				//DVASSERT(pen.x >= 0 && pen.y >= 0);
				
				FT_Error error = FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, 0, 0);
				if ( !error )
				{
					FT_BitmapGlyph  bit = (FT_BitmapGlyph)image;
					FT_Bitmap * bitmap = &bit->bitmap;

					//TODO: optimize
					int32 realH = Min((int32)bitmap->rows, (int32)(bufHeight - pen.y));
					int32 realW = Min((int32)bitmap->width, (int32)(bufWidth - pen.x)); 
					for(int32 h = 0; h < realH; h++)
					{
						for(int32 w = 0; w < realW; w++)
						{
							int lowBoundX = pen.x+w;
							int lowBoundY = pen.y+h;
							
							if(lowBoundX < 0 || lowBoundY < 0)
							{
								continue;
							}
						
							int16 oldPix = bitmap->buffer[h*bitmap->width+w];
							
							uint8 preAlpha = (oldPix*a)>>8;

							int32 ind = (pen.y+h)*bufWidth + (pen.x+w);
							
							uint8 prevA = (resultBuf[ind] & 0xf)<<4;
							uint8 prevR = ((resultBuf[ind]>>12) & 0xf)<<4;
							uint8 prevG = ((resultBuf[ind]>>8) & 0xf)<<4;
							uint8 prevB = ((resultBuf[ind]>>4) & 0xf)<<4;
							
							//		    source		        destination
							//		    one			        1-srcAlpha
							uint8 tempA = ((preAlpha))+(((256-preAlpha)*prevA)>>8);
							uint8 tempR = ((preAlpha*r)>>8)+(((256-preAlpha)*prevR)>>8); 
							uint8 tempG = ((preAlpha*g)>>8)+(((256-preAlpha)*prevG)>>8);
							uint8 tempB = ((preAlpha*b)>>8)+(((256-preAlpha)*prevB)>>8);
							
							resultBuf[ind] = (
							(((tempR)>>4)<<12) |
							(((tempG)>>4)<<8) | 
							(((tempB)>>4)<<4) | 
							((tempA)>>4)); 
						}
					}
					FT_Done_Glyph( image );
				}
			}
		}
		else
		{
		
			for(uint32 n = 0; n < num_glyphs; n++)
			{
				FT_Glyph   image;
				FT_Vector  pen;
				
				image = glyphs[n];
				
				pen.x = pos[n].x;
				pen.y = penY - pos[n].y + ((face->size->metrics.ascender + (1<<5))>>6);
				
				//DVASSERT(pen.x >= 0 && pen.y >= 0);
				
	//			if(pen.x > bufWidth || pen.y > bufHeight)
	//			{
	//				continue;
	//			}
				
				FT_Error error = FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, 0, 0);
				if ( !error )
				{
					FT_BitmapGlyph  bit = (FT_BitmapGlyph)image;
					FT_Bitmap * bitmap = &bit->bitmap;

					//TODO: optimize
					int32 realH = Min((int32)bitmap->rows, (int32)(bufHeight - pen.y));
					int32 realW = Min((int32)bitmap->width, (int32)(bufWidth - pen.x)); 
					for(int32 h = 0; h < realH; h++)
					{
						for(int32 w = 0; w < realW; w++)
						{
							int lowBoundX = pen.x+w;
							int lowBoundY = pen.y+h;
							
							if(lowBoundX < 0 || lowBoundY < 0)
							{
								continue;
							}
							
							int oldPix = bitmap->buffer[h*bitmap->width+w];
							
							uint8 preAlpha = (oldPix*a)>>8;
							if(preAlpha)
							{
								int32 revAlpha = 256-preAlpha;
								int32 ind = (pen.y+h)*bufWidth + (pen.x+w);
								
								uint8 prevA = (resultBuf[ind] & 0xf)<<4;
								uint8 prevR = ((resultBuf[ind]>>12) & 0xf)<<4;
								uint8 prevG = ((resultBuf[ind]>>8) & 0xf)<<4;
								uint8 prevB = ((resultBuf[ind]>>4) & 0xf)<<4;
								
								//		    source		        destination
								//		    one			        1-srcAlpha
								uint8 tempA = ((preAlpha)+((revAlpha*prevA)>>8));
								uint8 tempR = ((preAlpha*r)+(revAlpha*prevR))>>8; 
								uint8 tempG = ((preAlpha*g)+(revAlpha*prevG))>>8;
								uint8 tempB = ((preAlpha*b)+(revAlpha*prevB))>>8;
								resultBuf[ind] = (
												  (((tempR)>>4)<<12) |
												  (((tempG)>>4)<<8) | 
												  (((tempB)>>4)<<4) | 
												  ((tempA)>>4)); 
								
								//							resultBuf[(pen.y+h)*bufWidth + (pen.x+w)] = (
								//							(((preAlpha*r)>>12)<<12) |
								//							(((preAlpha*g)>>12)<<8) | 
								//							(((preAlpha*b)>>12)<<4) | 
								//							((preAlpha)>>4)); 
							}
							
						}
					}
					FT_Done_Glyph( image );
				}
			}
		}
	}
	
	// VB: we must release not only converted glypths but original glypths also
	for (int idx = 0; idx < num_chars; idx++)                           
		FT_Done_Glyph( glyphs[idx] );    
	
	delete[] glyphs;
	delete[] pos;

	drawStringMutex.Unlock();
	if (!contentScaleIncluded) 
	{
		return Size2i((int32)(ceilf(Core::GetPhysicalToVirtualFactor() * (bbox.xMax-bbox.xMin))), (int32)(ceilf(Core::GetPhysicalToVirtualFactor() * (((face->size->metrics.ascender+ (1<<5))>>6)-bbox.yMin))));
	}
	return Size2i( bbox.xMax-bbox.xMin, ((face->size->metrics.ascender+ (1<<5))>>6)-bbox.yMin);
}


bool FTInternalFont::IsCharAvaliable(char16 ch)
{
	return FT_Get_Char_Index(face, ch) != 0;
}
	

uint32 FTInternalFont::GetFontHeight(float32 size)
{
	SetFTCharSize(size);
	return (uint32)(Core::GetPhysicalToVirtualFactor() * ((face->size->metrics.ascender >> 6) - (face->size->metrics.descender >> 6)));
}
	
void FTInternalFont::SetFTCharSize(float32 size)
{
	FT_Error error = FT_Set_Char_Size(
									  face,    /* handle to face object           */
									  0,       /* char_width in 1/64th of points  */
									  (int32)(size * 64),   /* char_height in 1/64th of points */
									  0,     /* horizontal device resolution    */
									  (FT_UInt)(Core::GetVirtualToPhysicalFactor() * Font::GetDPI()));   /* vertical device resolution      */
//	FT_Error error = FT_Set_Pixel_Sizes(face, 0, (int32)size);
	
	
	if(error) 
	{
		Logger::Error("FTInternalFont::FT_Set_Char_Size");
	}
	
}


int32 FTInternalFont::GetAscender(float32 size)
{
	SetFTCharSize(size);
	return (int32)(Core::GetPhysicalToVirtualFactor() * (face->size->metrics.ascender >> 6));
}

int32 FTInternalFont::GetDescender(float32 size)
{
	SetFTCharSize(size);
	return (int32)(Core::GetPhysicalToVirtualFactor() * (face->size->metrics.descender >> 6));
}

	
};