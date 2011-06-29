/*
 *  TestScreen.h
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __DAVAENGINE_UISPRITEEDITOR_H__
#define __DAVAENGINE_UISPRITEEDITOR_H__

#include "Base/BaseTypes.h"
#include "UI/UIList.h"
#include "UIScrollView.h"

namespace DAVA 
{
	
// comment: use namespace and standard prefix because probably this class can be moved to framework later
class UISpriteEditor: public UIControl
{
public:
	UISpriteEditor(const Rect &rect = Rect(), bool rectInAbsoluteCoordinates = false);
	~UISpriteEditor();
	
	UIControl * GetSpritePreview();
//	UISlider * GetFrameSlider();
	
	void SetRect(const Rect &rect, bool rectInAbsoluteCoordinates = false);
	void SetPreviewSprite(const String & spriteName);
	
	void LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader);
	void LoadFromYamlNodeCompleted();
	
	
private:
	void OnCurrentFrameChanged(BaseObject * obj, void * userData, void * systemData);
	

	// UIGrid * grid;
	UIScrollView * spriteScrollView;
	UIControl * spritePreview;
	UIStaticText * frameCount;
	UIStaticText * currentFrame;
	UISlider * frameSlider;
};
};

#endif // __DAVAENGINE_UISPRITEEDITOR_H__