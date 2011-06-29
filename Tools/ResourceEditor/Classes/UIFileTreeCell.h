/*
 *  TestScreen.h
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __DAVAENGINE_UIFILETREECELL_H__
#define __DAVAENGINE_UIFILETREECELL_H__


#include "Base/BaseTypes.h"
#include "UI/UIListCell.h"

namespace DAVA 
{
	
	
class UITreeItemInfo;
// comment: use namespace and standard prefix because probably this class can be moved to framework later
class UIFileTreeCell : public UIListCell
{
public:
	UIFileTreeCell(const Rect &rect, const String &cellIdentifier);
	virtual void Draw(const UIGeometricData &geometricData);
	virtual void SystemDraw(const UIGeometricData &geometricData);

//	UIControl * GetIcon();
//	UIStaticText * GetText();
	void SetItemInfo(UITreeItemInfo * entry);
	UITreeItemInfo * GetItemInfo();
protected:
//	UIControl * triangleControl;
//	UIControl * icon;
//	UIStaticText * text;
	UITreeItemInfo * itemInfo;
};	
};

#endif // __DAVAENGINE_UIFILETREECELL_H__