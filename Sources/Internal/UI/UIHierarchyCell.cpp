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
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#include "UI/UIHierarchyCell.h"

namespace DAVA 
{
//    REGISTER_CLASS(UIListCell);

UIHierarchyCell::UIHierarchyCell()
:   UIButton()
,	node(NULL)
,	identifier("")
,	cellStore(NULL)
,   text(NULL)
,   openButton(NULL)
{
}

UIHierarchyCell::UIHierarchyCell(const Rect &rect, const String &cellIdentifier)
:	UIButton(rect)
,	node(NULL)
,	identifier(cellIdentifier)
,	cellStore(NULL)
{
    text = new UIStaticText(Rect(15, 0, rect.dx - 15, rect.dy));
    AddControl(text);
    openButton = new UIButton(Rect(0, 0, 15, rect.dy));
    AddControl(openButton);
}

UIHierarchyCell::~UIHierarchyCell()
{
    SafeRelease(text);
    SafeRelease(openButton);
}

void UIHierarchyCell::WillDisappear()
{
    node = NULL;
}

const String & UIHierarchyCell::GetIdentifier()
{
    return identifier;
}

UIHierarchyNode *UIHierarchyCell::GetNode()
{
    return node;
}

//    UIListCell *UIListCell::CloneListCell()
//	{
//		return (UIListCell *)Clone();
//	}
//    
//	UIControl *UIListCell::Clone()
//	{
//		UIListCell *c = new UIListCell(GetRect(),identifier);
//		c->CopyDataFrom(this);
//		return c;
//	}
//    
//    void UIListCell::CopyDataFrom(UIControl *srcControl)
//	{
//        UIButton::CopyDataFrom(srcControl);
//        UIListCell *srcListCell = (UIListCell *)srcControl;
//        identifier = srcListCell->identifier;
//    }

//    void UIListCell::LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader)
//	{
//        YamlNode * identifierNode = node->Get("identifier");
//        if (identifierNode)
//        {
//            identifier = identifierNode->AsString();
//        }
//        UIButton::LoadFromYamlNode(node, loader);
//    }
};