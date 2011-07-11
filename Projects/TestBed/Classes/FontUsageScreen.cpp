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
#include "FontUsageScreen.h"

void FontUsageScreen::LoadResources()
{
	singleLineString = L"!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	singleLineString = L"!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHpqrstuvwxyz{|}~";
	singleLineString = L"123456 - Wave String!.W";
	
	//graphicsFont1 = GraphicsFont::Create("~res:/Fonts/myrpro.def", "~res:/Gfx/Fonts2/myrpro");
	//graphicsFont1 = GraphicsFont::Create("~res:/Fonts/korinna.def", "~res:/Gfx/Fonts2/korinna");
	graphicsFont1 = GraphicsFont::Create("~res:/Fonts/balderll_34.def", "~res:/Gfx/Fonts/balderll_34_menu");
	originalSize = graphicsFont1->GetSize();
	graphicsFont2 = GraphicsFont::Create("~res:/Fonts/jointby.def", "~res:/Gfx/Fonts2/jointby");
	//sprite = Sprite::Create("~res:/Gfx/Fonts2/jointby");
	
	ftFont1 = FTFont::Create("~res:/Fonts/bd_cartoon.ttf");
	ftFont1->SetSize(24.0f);
	ftFont1->SetColor(1.0f, 1.0f, 1.0f, 1.0f);	
	
	ftFont2 = FTFont::Create("~res:/Fonts/jointbypizzadude.ttf");
	ftFont2->SetSize(24.0f);
	ftFont2->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	textSpriteWithFreetypeSingleLineFont = TextBlock::Create(Vector2(900, 200));
	//graphicsFont1->SetSize(7.0f);
	
	textSpriteWithFreetypeSingleLineFont->SetFont(graphicsFont1);
	textSpriteWithFreetypeSingleLineFont->SetAlign(ALIGN_LEFT | ALIGN_VCENTER);
	//textSpriteWithFreetypeSingleLineFont->SetMultiline(true);
	textSpriteWithFreetypeSingleLineFont->SetText(singleLineString);
												  //L"Multi line text test1\nMulti line text bla bla test2\nMulti line text test3\n");
}


void FontUsageScreen::UnloadResources()
{
	SafeRelease(ftFont2);
	SafeRelease(ftFont1);
	SafeRelease(graphicsFont1);
	SafeRelease(graphicsFont2);
}

void FontUsageScreen::WillAppear()
{
}

void FontUsageScreen::WillDisappear()
{
	
}

void FontUsageScreen::Input(UIEvent * touch)
{
}

void FontUsageScreen::DrawGraphicsFont(GraphicsFont * font, Vector2 position, float32 fontSize)
{	  
	font->SetSize(fontSize);
	Size2i size = font->GetStringSize(singleLineString, 0);
	// Size2i size = font->GetStringSize(singleLineString);
	Rect rect(position.x, position.y, size.dx, size.dy);
	RenderManager::Instance()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
	RenderHelper::Instance()->DrawRect(rect);
	RenderHelper::Instance()->DrawLine(Vector2(position.x, rect.y + font->GetAscender()), Vector2(position.x + size.dx, rect.y + font->GetAscender()));

	RenderManager::Instance()->ResetColor();
	Size2i checkSize = font->DrawString(position.x, position.y, singleLineString);
	DVASSERT(checkSize.dx == size.dx && checkSize.dy == size.dy);
}

void FontUsageScreen::DrawFTFont(FTFont * ftFont, Vector2 pos, float32 fontSize)
{
	ftFont->SetSize(fontSize);
	Size2i size = ftFont->GetStringSize(singleLineString);
	Rect rect(pos.x, pos.y, size.dx, size.dy);
	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	//RenderManager::Instance()->DrawRect(rect);
	
	RenderHelper::Instance()->DrawLine(Vector2(pos.x, pos.y + ftFont->GetAscender()), Vector2(pos.x + size.dx, pos.y + ftFont->GetAscender()));
	
	int32 dx = 960;
	int32 dy = 40;
	int16 * buf;
	int bsz = dx * dy;
	buf = new int16[bsz];
	memset(buf, 0, dx * dy * sizeof(int16));
	ftFont->DrawStringToBuffer(buf, dx, dy, 0, 0, 0, singleLineString, true);
	Texture *tex = Texture::CreateTextFromData(Texture::FORMAT_RGBA4444, (uint8*)buf, dx, dy, "");
	delete[] buf;
	
	Sprite * ftFontSprite = Sprite::CreateFromTexture(tex, 0, 0, dx, dy);
//	if(!isMultilineEnabled)
//	{
//		textSprite->SetDefaultPivotPoint(0.0f, (float32)(font->GetBaseline() - h));
//	}

	SafeRelease(tex);
	
	ftFontSprite->SetPosition(pos.x, pos.y);
	ftFontSprite->Draw();
	SafeRelease(ftFontSprite); 
}

void FontUsageScreen::Draw(const UIGeometricData &geometricData)
{
	//DrawFTFont(ftFont1, Vector2(1, 10), 24);
	DrawGraphicsFont(graphicsFont1, Vector2(10, 50), originalSize);
	DrawGraphicsFont(graphicsFont1, Vector2(10, 130), originalSize / 2);
	DrawGraphicsFont(graphicsFont1, Vector2(10, 160), originalSize / 3);
	DrawGraphicsFont(graphicsFont1, Vector2(10, 200), originalSize / 5);


	Sprite * sp = textSpriteWithFreetypeSingleLineFont->GetSprite();
	sp->SetPosition(10, 300);
	sp->Draw();
	
}
