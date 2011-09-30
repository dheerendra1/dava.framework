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
#include "Render/2D/FTFont.h"
#include "Render/2D/FontManager.h"
#include "FileSystem/Logger.h"
#include "Utils/Utils.h"
#include "Debug/DVAssert.h"
#include "FileSystem/File.h"
#include "Core/Core.h"

#include <ft2build.h>
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H


namespace DAVA
{

Map<String,FTInternalFont*> fontMap;

class FTInternalFont : public BaseObject
{
	friend class FTFont;
	String fontPath;
	uint8 * memoryFont;
	uint32 memoryFontSize;
private:
	FTInternalFont(const String& path);
	virtual ~FTInternalFont();

public:
	FT_Face face;
	Size2i DrawString(const WideString& str, void * buffer, int32 bufWidth, int32 bufHeight, 
		uint8 r, uint8 g, uint8 b, uint8 a, 
		float32 size, bool realDraw, 
		int32 offsetX, int32 offsetY,
		int32 justifyWidth, int32 spaceAddon,
		Vector<int32> *charSizes = NULL,
		bool contentScaleIncluded = false);
	uint32 GetFontHeight(float32 size);
	int32 GetAscender(float32 size);
	int32 GetDescender(float32 size);

	bool IsCharAvaliable(char16 ch);

	void SetFTCharSize(float32 size);

	virtual int32 Release();

private:
	static Mutex drawStringMutex;

	struct Glyph
	{
		FT_UInt		index;
		FT_Glyph	image;    /* the glyph image */

		FT_Pos		delta;    /* delta caused by hinting */
	};
	Vector<Glyph> glyphs;

	void ClearString();
	int32 LoadString(const WideString& str);
	void Prepare(FT_Vector * advances);

	inline FT_Pos Round(FT_Pos val);
};

FTFont::FTFont(FTInternalFont* _internalFont)
:	Font(),
	r((uint8)(color.r * 255.0f)),  
	g((uint8)(color.g * 255.0f)), 
	b((uint8)(color.b * 255.0f)), 
	a((uint8)(color.a * 255.0f))
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


void FTFont::SetColor(const Color & color)
{
	SetColor(color.r, color.g, color.b, color.a);
}
	
Size2i FTFont::DrawStringToBuffer(void * buffer, int32 bufWidth, int32 bufHeight, int32 offsetX, int32 offsetY, int32 justifyWidth, int32 spaceAddon, const WideString& str, bool contentScaleIncluded )
{
	return internalFont->DrawString(str, buffer, bufWidth, bufHeight, r, g, b, a, size, true, offsetX, offsetY, justifyWidth, spaceAddon, NULL, contentScaleIncluded );
}

Size2i FTFont::GetStringSize(const WideString& str, Vector<int32> *charSizes)
{
	return internalFont->DrawString(str, 0, 0, 0, 0, 0, 0, 0, size, false, 0, 0, 0, 0, charSizes);
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
					float32 size, bool realDraw, 
					int32 offsetX, int32 offsetY,
					int32 justifyWidth, int32 spaceAddon,
					Vector<int32> *charSizes,
					bool contentScaleIncluded )
{
	drawStringMutex.Lock();

	FT_Error error;

	float32 virtualToPhysicalFactor = Core::GetVirtualToPhysicalFactor();
	
	if(!contentScaleIncluded) 
	{
		bufWidth = (int32)(virtualToPhysicalFactor * bufWidth);
		bufHeight = (int32)(virtualToPhysicalFactor * bufHeight);
		offsetY = (int32)(virtualToPhysicalFactor * offsetY);
		offsetX = (int32)(virtualToPhysicalFactor * offsetX);
	}

	SetFTCharSize(size);

	FT_Vector pen;
	pen.x = offsetX<<6;
	pen.y = offsetY<<6;
	pen.y -= face->bbox.yMin;//bring baseline up

	// virtualToPhysicalFactor scaling
	{
		FT_Fixed mul = 1<<16;
		FT_Matrix matrix;
		matrix.xx = (FT_Fixed)(virtualToPhysicalFactor*mul);
		matrix.xy = 0;
		matrix.yx = 0;
		matrix.yy = (FT_Fixed)(virtualToPhysicalFactor*mul);
		FT_Set_Transform(face, &matrix, 0);
	}

	int16 * resultBuf = (int16*)buffer;

	int32 spacesCount = LoadString(str);
	int32 strLen = str.length();
	FT_Vector * advances = new FT_Vector[strLen];
	Prepare(advances);

	int32 lastRight = 0; //charSizes helper
	int32 justifyOffset = 0;
	
	for(int32 i = 0; i < strLen; ++i)
	{
		Glyph		& glyph = glyphs[i];
		FT_Glyph	image;
		FT_BBox		bbox;

		if (!glyph.image)
			continue;

		error = FT_Glyph_Copy(glyph.image, &image);
		if(error)
			continue;

		if(!error)
			error = FT_Glyph_Transform(image, 0, &pen);

		if(error)
		{
			FT_Done_Glyph( image );
			continue;
		}

		pen.x += advances[i].x;
		pen.y += advances[i].y;

		FT_Glyph_Get_CBox(image, FT_GLYPH_BBOX_PIXELS, &bbox);

		int32 baseSize = ((int32)((virtualToPhysicalFactor*face->bbox.yMax-virtualToPhysicalFactor*face->bbox.yMin))>>6);
		int32 multilineOffsetY = baseSize+offsetY*2;
		if(!realDraw || (bbox.xMax>0 && bbox.yMax>0 && bbox.xMin<bufWidth && bbox.yMin < bufHeight))
		{
 			error = FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, 0, 0);
			if(!error)
			{
				FT_BitmapGlyph  bit = (FT_BitmapGlyph)image;
				FT_Bitmap * bitmap = &bit->bitmap;

				int32 left = bit->left;
				int32 top = multilineOffsetY-bit->top;
				int32 width = bitmap->width;
				int32 height = bitmap->rows;
				if(charSizes)
				{
					if(0 == width)
					{
						charSizes->push_back(0);
					}
					else if(charSizes->empty())
					{
						charSizes->push_back(width);
						lastRight = width;
					}
					else
					{
						int32 sizesSize = charSizes->size();
						int32 value = left+width-lastRight;
						lastRight += value;
						charSizes->push_back(value);
					}
				}

				if(realDraw)
				{
					int32 realH = Min((int32)bitmap->rows, (int32)(bufHeight - top));
					int32 realW = Min((int32)bitmap->width, (int32)(bufWidth - left)); 
					for(int32 h = 0; h < realH; h++)
					{
						for(int32 w = 0; w < realW; w++)
						{
							int oldPix = bitmap->buffer[h*bitmap->pitch+w];

							uint8 preAlpha = (oldPix*a)>>8;
							if(preAlpha)
							{
								int32 revAlpha = 256-preAlpha;
								int32 ind = (h+top)*bufWidth + ((left)+w);

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
							}

						}
					}
				}
			}
		}

		FT_Done_Glyph(image);
	}
	
	if(contentScaleIncluded) 
	{
		return Size2i(advances[strLen-1].x>>6, GetFontHeight(size));
	}
	else
	{
		return Size2i((int32)ceilf(Core::GetPhysicalToVirtualFactor()*(advances[strLen-1].x>>6)), (int32)ceilf(Core::GetPhysicalToVirtualFactor()*GetFontHeight(size)));
	}

	//
	//bool use_kerning = (FT_HAS_KERNING(face) > 0);
	//int32 previous = 0;
	//
	//FT_UInt num_glyphs = 0;
	//
	//int32 num_chars = (int32)str.length();
	//FT_Glyph * glyphs = new FT_Glyph[num_chars];
	//FT_Vector * pos = new FT_Vector[num_chars];
	//
	////calculate string rect
	//FT_BBox  bbox;
	//
	//
	///* initialize string bbox to "empty" values */
	//bbox.xMin = bbox.yMin =  32000;
	//bbox.xMax = bbox.yMax = -32000;
	//int prevSz = 0;
	//int spacesCount = 0;
	//
	//for(int32 n = 0; n < num_chars; n++)
	//{
	//	/* convert character code to glyph index */
	//	if (str[n] == L' ') 
	//	{
	//		spacesCount++;
	//	}
	//	int32 glyph_index = FT_Get_Char_Index(face, str[n]);
	//	
	//	/* retrieve kerning distance and move pen position */
	//	if (use_kerning && previous && glyph_index)
	//	{
	//		FT_Vector  delta;
	//		FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_UNFITTED, &delta );
	//		penX += delta.x >> 6;
	//	}
	//	
	//	/* store current pen position */
	//	pos[num_glyphs].x = penX;
	//	
	//	
	//	/* load glyph image into the slot without rendering */
	//	FT_Error error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
	//	if ( error )
	//	{
	//		if(charSizes)
	//		{
	//			(*charSizes).push_back(0);
	//		}
	//		continue;  /* ignore errors, jump to next glyph */
	//	}
	//	
	//	/* extract glyph image and store it in our table */
	//	error = FT_Get_Glyph( face->glyph, &glyphs[num_glyphs] );
	//	if ( error )
	//	{
	//		if(charSizes)
	//		{
	//			(*charSizes).push_back(0);
	//		}
	//		continue;  /* ignore errors, jump to next glyph */
	//	}
	//	
	//	/* increment pen position */
	//	//penX += face->glyph->metrics.width >> 6;
	//	penX += (face->glyph->metrics.horiAdvance + (1<<5)) >> 6;
	//	//pos[num_glyphs].y = penY - ((face->glyph->metrics.height - face->glyph->metrics.horiBearingY) >>6);
	//	pos[num_glyphs].y = ((face->glyph->metrics.horiBearingY + (1<<5)) >>6);
	//	/* record current glyph index */
	//	previous = glyph_index;
	//	
	//	
	//	
	//	/* for each glyph image, compute its bounding box, */
	//	/* translate it, and grow the string bbox          */
	//	FT_BBox  glyph_bbox;
	//	
	//	
	//	FT_Glyph_Get_CBox(glyphs[num_glyphs], ft_glyph_bbox_pixels, &glyph_bbox );
	//	
	//	if(!glyph_bbox.xMin && !glyph_bbox.xMax && !glyph_bbox.yMin && !glyph_bbox.yMax)
	//	{
	//		//SPACE symbol
	//		num_glyphs++;
	//		if(charSizes)
	//		{
	//			(*charSizes).push_back(0);
	//		}
	//		continue;
	//	}
	//	glyph_bbox.xMin += pos[num_glyphs].x;
	//	glyph_bbox.xMax += pos[num_glyphs].x;
	//	
	//	if(isShadow)
	//	{
	//		if(shadowOffset.x > 0) glyph_bbox.xMax += (FT_Pos)shadowOffset.x;
	//		if(shadowOffset.x < 0) glyph_bbox.xMin += (FT_Pos)shadowOffset.x;
	//		if(shadowOffset.y > 0) glyph_bbox.yMax += (FT_Pos)shadowOffset.y;
	//		if(shadowOffset.y < 0) glyph_bbox.yMin += (FT_Pos)shadowOffset.y;
	//	}
	//	
	//	if (glyph_bbox.xMin < bbox.xMin)
	//		bbox.xMin = glyph_bbox.xMin;
	//	
	//	if (glyph_bbox.yMin < bbox.yMin)
	//		bbox.yMin = glyph_bbox.yMin;
	//	
	//	if (glyph_bbox.xMax > bbox.xMax)
	//		bbox.xMax = glyph_bbox.xMax;
	//	
	//	if (glyph_bbox.yMax > bbox.yMax)
	//		bbox.yMax = glyph_bbox.yMax;
	//	
	//	
	//	if(charSizes)
	//	{
	//		int sz = bbox.xMax-bbox.xMin;
	//		(*charSizes).push_back(sz - prevSz);
	//		prevSz = sz;
	//	}
	//	/* increment number of glyphs */
	//	num_glyphs++;
	//}


	////if(bbox.yMin > 0) bbox.yMin = 0;
	//if(bbox.xMin > 0) bbox.xMin = 0;

	///* check that we really grew the string bbox */
	//if ( bbox.xMin > bbox.xMax )
	//{
	//	bbox.xMin = 0;
	//	bbox.yMin = 0;
	//	bbox.xMax = 0;
	//	bbox.yMax = 0;
	//}
	//
	//if(realDraw)
	//{
	//	if (justifyWidth > 0 && spacesCount > 0) 
	//	{
	//		int oversize = justifyWidth - (bbox.xMax - bbox.xMin);
	//		int spaceAdd = oversize / spacesCount;
	//		int totalAdd = 0;
	//		if (oversize > 0) 
	//		{
	//			oversize -= spacesCount * spaceAdd;
	//			for(int32 n = 0; n < num_chars; n++)
	//			{
	//				/* convert character code to glyph index */
	//				if (str[n] == L' ') 
	//				{
	//					if (oversize > 0)
	//					{
	//						totalAdd += spaceAdd + 1;
	//						oversize--;
	//					}
	//					else 
	//					{
	//						totalAdd += spaceAdd;
	//					}
	//					
	//				}
	//				pos[n].x = pos[n].x + totalAdd;
	//			}
	//		}
	//	}
	//	//create bufer
	//	//uint32 width = 256;//bbox.xMax - bbox.xMin;
	//	//uint32 height = 256;//bbox.yMax - bbox.yMin;
	//	//int16 * resultBuf = new int16[bufWidth*bufHeight];
	//	
	//	// VB: Please be patient --- please do not clear the buffer here 
	//	// because it can break multiline text
	//	// Dizz: I'm patient just as a boa
	//	
	//	{
	//	
	//		for(uint32 n = 0; n < num_glyphs; n++)
	//		{
	//			FT_Glyph   image;
	//			FT_Vector  pen;
	//			
	//			image = glyphs[n];
	//			
	//			pen.x = pos[n].x;
	//			pen.y = penY - pos[n].y + ((face->size->metrics.ascender + (1<<5))>>6);
	//			
	//			//DVASSERT(pen.x >= 0 && pen.y >= 0);
	//			
	////			if(pen.x > bufWidth || pen.y > bufHeight)
	////			{
	////				continue;
	////			}
	//			
	//			FT_Error error = FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, 0, 0);
	//			if ( !error )
	//			{
	//				FT_BitmapGlyph  bit = (FT_BitmapGlyph)image;
	//				FT_Bitmap * bitmap = &bit->bitmap;

	//				//TODO: optimize
	//				int32 realH = Min((int32)bitmap->rows, (int32)(bufHeight - pen.y));
	//				int32 realW = Min((int32)bitmap->width, (int32)(bufWidth - pen.x)); 
	//				for(int32 h = 0; h < realH; h++)
	//				{
	//					for(int32 w = 0; w < realW; w++)
	//					{
	//						int lowBoundX = pen.x+w;
	//						int lowBoundY = pen.y+h;
	//						
	//						if(lowBoundX < 0 || lowBoundY < 0)
	//						{
	//							continue;
	//						}
	//						
	//						int oldPix = bitmap->buffer[h*bitmap->width+w];
	//						
	//						uint8 preAlpha = (oldPix*a)>>8;
	//						if(preAlpha)
	//						{
	//							int32 revAlpha = 256-preAlpha;
	//							int32 ind = (pen.y+h)*bufWidth + (pen.x+w);
	//							
	//							uint8 prevA = (resultBuf[ind] & 0xf)<<4;
	//							uint8 prevR = ((resultBuf[ind]>>12) & 0xf)<<4;
	//							uint8 prevG = ((resultBuf[ind]>>8) & 0xf)<<4;
	//							uint8 prevB = ((resultBuf[ind]>>4) & 0xf)<<4;
	//							
	//							//		    source		        destination
	//							//		    one			        1-srcAlpha
	//							uint8 tempA = ((preAlpha)+((revAlpha*prevA)>>8));
	//							uint8 tempR = ((preAlpha*r)+(revAlpha*prevR))>>8; 
	//							uint8 tempG = ((preAlpha*g)+(revAlpha*prevG))>>8;
	//							uint8 tempB = ((preAlpha*b)+(revAlpha*prevB))>>8;
	//							resultBuf[ind] = (
	//											  (((tempR)>>4)<<12) |
	//											  (((tempG)>>4)<<8) | 
	//											  (((tempB)>>4)<<4) | 
	//											  ((tempA)>>4)); 
	//							
	//							//							resultBuf[(pen.y+h)*bufWidth + (pen.x+w)] = (
	//							//							(((preAlpha*r)>>12)<<12) |
	//							//							(((preAlpha*g)>>12)<<8) | 
	//							//							(((preAlpha*b)>>12)<<4) | 
	//							//							((preAlpha)>>4)); 
	//						}
	//						
	//					}
	//				}
	//				FT_Done_Glyph( image );
	//			}
	//		}
	//	}
	//}
	//
	//// VB: we must release not only converted glypths but original glypths also
	//for (int idx = 0; idx < num_chars; idx++)                           
	//	FT_Done_Glyph( glyphs[idx] );    
	//
	//delete[] glyphs;
	//delete[] pos;

	//drawStringMutex.Unlock();
	//if (!contentScaleIncluded) 
	//{
	//	return Size2i((int32)(ceilf(Core::GetPhysicalToVirtualFactor() * (bbox.xMax-bbox.xMin))), (int32)(ceilf(Core::GetPhysicalToVirtualFactor() * (((face->size->metrics.ascender+ (1<<5))>>6)-bbox.yMin))));
	//}
	//return Size2i( bbox.xMax-bbox.xMin, ((face->size->metrics.ascender+ (1<<5))>>6)-bbox.yMin);
}


bool FTInternalFont::IsCharAvaliable(char16 ch)
{
	return FT_Get_Char_Index(face, ch) != 0;
}
	

uint32 FTInternalFont::GetFontHeight(float32 size)
{
	SetFTCharSize(size);
	return ((face->bbox.yMax-face->bbox.yMin)>>6);
	//return (uint32)(Core::GetPhysicalToVirtualFactor() * ((face->size->metrics.ascender >> 6) - (face->size->metrics.descender >> 6)));
}
	
void FTInternalFont::SetFTCharSize(float32 size)
{
	FT_Error error = FT_Set_Char_Size(
									  face,    /* handle to face object           */
									  0,       /* char_width in 1/64th of points  */
									  (int32)(size * 64),   /* char_height in 1/64th of points */
									  0,     /* horizontal device resolution    */
									  (FT_UInt)Font::GetDPI()/*(Core::GetVirtualToPhysicalFactor() * Font::GetDPI())*/);   /* vertical device resolution      */
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

void FTInternalFont::Prepare(FT_Vector * advances)
{
	FT_Vector	* prevAdvance = 0;
	FT_Vector	extent = {0, 0};
	FT_UInt		prevIndex   = 0;
	bool		useKerning = (FT_HAS_KERNING(face) > 0);
	int32		size = glyphs.size();

	for(int32 i = 0; i < size; ++i)
	{
		Glyph & glyph = glyphs[i];

		advances[i] = glyph.image->advance;
		advances[i].x >>= 10;
		advances[i].y >>= 10;

		if(prevAdvance)
		{
			//prevAdvance->x += track_kern;

			if(useKerning)
			{
				FT_Vector  kern;

				FT_Get_Kerning(face, prevIndex, glyph.index, FT_KERNING_UNFITTED, &kern );

				prevAdvance->x += kern.x;
				prevAdvance->y += kern.y;

				//if(sc->kerning_mode > KERNING_MODE_NORMAL)
					prevAdvance->x += glyph.delta;
			}

			//if(handle->hinted)
			//{
			//	prevAdvance->x = Round(prevAdvance->x);
			//	prevAdvance->y = Round(prevAdvance->y);
			//}

			extent.x += prevAdvance->x;
			extent.y += prevAdvance->y;
		}

		prevIndex   = glyph.index;
		prevAdvance = &advances[i];
	}

	if(prevAdvance)
	{
		//if(handle->hinted)
		//{
		//	prevAdvance->x = Round(prevAdvance->x);
		//	prevAdvance->y = Round(prevAdvance->y);
		//}

		extent.x += prevAdvance->x;
		extent.y += prevAdvance->y;
	}

	advances[size-1] = extent;
}

void FTInternalFont::ClearString()
{
	int32 size = glyphs.size();
	for(int32 i = 0; i < size; ++i)
	{
		if(glyphs[i].image)
		{
			FT_Done_Glyph(glyphs[i].image);
		}
	}

	glyphs.clear();
}

int32 FTInternalFont::LoadString(const WideString& str)
{
	ClearString();

	int32 spacesCount = 0;
	FT_Pos prevRsbDelta = 0;
	int32 size = str.size();
	for(int32 i = 0; i < size; ++i)
	{
		if( L' ' == str[i])
		{
			spacesCount++;
		}

		Glyph glyph;
		glyph.index = FT_Get_Char_Index(face, str[i]);
		if (!FT_Load_Glyph( face, glyph.index, FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING)  &&
			!FT_Get_Glyph(face->glyph, &glyph.image))
		{
			FT_Glyph_Metrics*  metrics = &face->glyph->metrics;

			if(prevRsbDelta - face->glyph->lsb_delta >= 32 )
				glyph.delta = -1 << 6;
			else if(prevRsbDelta - face->glyph->lsb_delta < -32)
				glyph.delta = 1 << 6;
			else
				glyph.delta = 0;
		}

		glyphs.push_back(glyph);
	}

	return spacesCount;
}

FT_Pos FTInternalFont::Round(FT_Pos val)
{
	return (((val) + 32) & -64);
}

	
};
