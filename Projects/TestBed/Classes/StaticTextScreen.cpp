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
#include "StaticTextScreen.h"


void StaticTextScreen::LoadResources()
{
	Font *fnt;
	fnt = FTFont::Create("~res:/Fonts/helios.otf");
//	fnt = GraphicsFont::Create("~res:/Fonts/korinna.def", "~res:/Gfx/Fonts2/korinna");
	fnt->SetSize(16);
//	fnt->SetSize(12);
//	fnt->SetVerticalSpacing(-1);
	fnt->SetColor(.82f, .81f, 0.73f, 1.0f);
	float32 sizeX = 150;
	float32 sizeY = 80;

	int32 ii = fnt->GetFontHeight();
    
	UIControl *tc = new UIControl(Rect(0, 0, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0.f, 0.f, 0.f, 1.0f);
	UIStaticText *tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
//	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_LEFT|ALIGN_TOP);
	tx->SetText(L"gAcademy Исследовать");
//	tx->SetText(L"This text is aligned by the Left and Top borders.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);
	
	tc = new UIControl(Rect((size.x - sizeX)/2, 0, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_HCENTER|ALIGN_TOP);
//	tx->SetText(L"abab.");
	tx->SetText(L"This text is aligned byyyi the Center and Top borders.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);
	
	tc = new UIControl(Rect(size.x - sizeX, 0, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_RIGHT|ALIGN_TOP);
//	tx->SetText(L"apap.");
	tx->SetText(L"This text is aligned by the Right and Top borders.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);

	
	
	tc = new UIControl(Rect(0, (size.y - sizeY)/2, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_LEFT|ALIGN_VCENTER);
//	tx->SetText(L"apap.");
	tx->SetText(L"This text is aligned by the Left and Center borders.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);
	
	tc = new UIControl(Rect((size.x - sizeX)/2, (size.y - sizeY)/2, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_HCENTER|ALIGN_VCENTER);
//	tx->SetText(L"abpbs");
	tx->SetText(L"This text is aligned by the Center and Center borders.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);
	
	tc = new UIControl(Rect(size.x - sizeX, (size.y - sizeY)/2, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_RIGHT|ALIGN_VCENTER);
//	tx->SetText(L"aaaa.");
	tx->SetText(L"This text is aligned by the Right and Center borders.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);

	
	tc = new UIControl(Rect(0, size.y - sizeY, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_LEFT|ALIGN_BOTTOM);
	tx->SetText(L"This text is aligned by the Left and Bottom borders.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);
	
	tc = new UIControl(Rect((size.x - sizeX)/2, size.y - sizeY, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_HCENTER|ALIGN_BOTTOM);
	tx->SetText(L"This text is aligned by the Center and Bottom borders.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);
	
	tc = new UIControl(Rect(size.x - sizeX, size.y - sizeY, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_RIGHT|ALIGN_BOTTOM);
	tx->SetText(L"No desc elems.uj");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);
	
	
	
	tc = new UIControl(Rect((size.x - sizeX)/2, (size.y - sizeY)/2 - 70, sizeX, sizeY));
	tc->SetSpriteDrawType(UIControlBackground::DRAW_FILL);
	tc->GetBackground()->color = Color(0,0,0.4f,1.0f);
	tx = new UIStaticText(Rect(0, 0, sizeX, sizeY));
	tx->SetFont(fnt);
	tx->SetMultiline(true);
	tx->SetAlign(ALIGN_HJUSTIFY|ALIGN_TOP);
	tx->SetText(L"This text is Justify and it's looks little better than the others. aja. AVA.");
	tc->AddControl(tx);
	AddControl(tc);
	SafeRelease(tc);
	SafeRelease(tx);
	
	
	SafeRelease(fnt);
}