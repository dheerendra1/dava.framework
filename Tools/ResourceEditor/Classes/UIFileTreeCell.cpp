/*
 *  TestScreen.cpp
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "UIFileTreeCell.h"

namespace DAVA 
{
UIFileTreeCell::UIFileTreeCell(const Rect &rect, const String &cellIdentifier)
:	UIListCell(rect, cellIdentifier)
{
	FTFont * fnt = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
	fnt->SetColor(0, 0, 0, 1);
	fnt->SetSize(14);
	SetStateFont(STATE_NORMAL, fnt);
	SafeRelease(fnt);
	
	
	//Rect iconSize = Rect(0, 0, rect.dy, rect.dy);
	//icon = new UIControl(rect);
	//text = new UIStaticText(rect);
	
	
	// icon = UIControl(, <#bool rectInAbsoluteCoordinates#>)
	// 
	//GetStateBackground(STATE_NORMAL)->SetDrawType(UIControlBackground::DRAW_FILL);
	//GetStateBackground(STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 1.0f));
	//SetStateText(UIControl::STATE_NORMAL, L"Mega test!");

}
	
void UIFileTreeCell::SetItemInfo(UITreeItemInfo * _itemInfo)
{
	itemInfo = _itemInfo;
}

UITreeItemInfo * UIFileTreeCell::GetItemInfo()
{
	return itemInfo;
}
	
void UIFileTreeCell::SystemDraw(const UIGeometricData &geometricData)
{
	UIListCell::SystemDraw(geometricData);
}
	
void UIFileTreeCell::Draw(const UIGeometricData &geometricData)
{
	UIListCell::Draw(geometricData);
}

};

