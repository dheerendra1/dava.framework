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
#include "UI/UIYamlLoader.h"
#include "Base/ObjectFactory.h"
#include "Platform/SystemTimer.h"
#include "UI/UIControl.h"
#include "FileSystem/YamlParser.h"
#include "Render/2D/GraphicsFont.h"

namespace DAVA 
{
    
int32 UIYamlLoader::GetDrawTypeFromNode(YamlNode * drawTypeNode)
{
    int32 ret = UIControlBackground::DRAW_ALIGNED;
    if(!drawTypeNode)
        return ret;
    
    const String & type = drawTypeNode->AsString();
    
    if("DRAW_ALIGNED" == type) ret = UIControlBackground::DRAW_ALIGNED;
    if("DRAW_SCALE_TO_RECT" == type) ret = UIControlBackground::DRAW_SCALE_TO_RECT;
    if("DRAW_SCALE_PROPORTIONAL" == type) ret = UIControlBackground::DRAW_SCALE_PROPORTIONAL;
    if("DRAW_FILL" == type) ret = UIControlBackground::DRAW_FILL;
    if("DRAW_STRETCH_HORIZONTAL" == type) ret = UIControlBackground::DRAW_STRETCH_HORIZONTAL;
    if("DRAW_STRETCH_VERTICAL" == type) ret = UIControlBackground::DRAW_STRETCH_VERTICAL;
    if("DRAW_STRETCH_BOTH" == type) ret = UIControlBackground::DRAW_STRETCH_BOTH;
    
    return ret;
}

int32 UIYamlLoader::GetColorInheritTypeFromNode(YamlNode * colorInheritNode)
{
	int32 ret = UIControlBackground::COLOR_IGNORE_PARENT;
	if(!colorInheritNode)
		return ret;

	const String & type = colorInheritNode->AsString();

	if("COLOR_MULTIPLY_ON_PARENT" == type) ret = UIControlBackground::COLOR_MULTIPLY_ON_PARENT;
	if("COLOR_ADD_TO_PARENT" == type) ret = UIControlBackground::COLOR_ADD_TO_PARENT;
	if("COLOR_REPLACE_TO_PARENT" == type) ret = UIControlBackground::COLOR_REPLACE_TO_PARENT;
	if("COLOR_IGNORE_PARENT" == type) ret = UIControlBackground::COLOR_IGNORE_PARENT;
	if("COLOR_MULTIPLY_ALPHA_ONLY" == type) ret = UIControlBackground::COLOR_MULTIPLY_ALPHA_ONLY;
	if("COLOR_REPLACE_ALPHA_ONLY" == type) ret = UIControlBackground::COLOR_REPLACE_ALPHA_ONLY;

	return ret;
}
	
int32 UIYamlLoader::GetAlignFromYamlNode(YamlNode * alignNode)
{
	if (!alignNode)return ALIGN_HCENTER | ALIGN_VCENTER;
	
	Vector<YamlNode*> & vec = alignNode->AsVector();
	if (vec.size() != 2)return ALIGN_HCENTER | ALIGN_VCENTER;
	
	const String & horzAlign = vec[0]->AsString();
	const String & vertAlign = vec[1]->AsString();
	
	int32 align = 0;
	if (horzAlign == "LEFT")align |= ALIGN_LEFT;
	else if (horzAlign == "HCENTER")align |= ALIGN_HCENTER;
	else if (horzAlign == "RIGHT")align |= ALIGN_RIGHT;
	
	if (vertAlign == "TOP")align |= ALIGN_TOP;
	else if (vertAlign == "VCENTER")align |= ALIGN_VCENTER;
	else if (vertAlign == "BOTTOM")align |= ALIGN_BOTTOM;
	
	return align;
}
	
bool UIYamlLoader::GetBoolFromYamlNode(YamlNode * node, bool defaultValue)
{
	if (!node)return defaultValue;
	
	const String & value = node->AsString();
	if (value == "yes")return true;
	else if (value == "no")return false;
	else if (value == "true")return true;
	
	return false;
}

int32 HexCharToInt(char c)
{
	if ((c >= '0') && (c <= '9'))return c - '0';
	else if ((c >= 'a') && (c <= 'f'))return c - 'a' + 10;
	else if ((c >= 'A') && (c <= 'F'))return c - 'A' + 10;
	return 0;
}

Color UIYamlLoader::GetColorFromYamlNode(YamlNode * node)
{
	if (node->GetType() == YamlNode::TYPE_ARRAY)
	{
		if (node->GetCount() == 4)
			return Color(node->Get(0)->AsFloat(), node->Get(1)->AsFloat(), node->Get(2)->AsFloat(), node->Get(3)->AsFloat());
		else return Color(1.0f, 1.0f, 1.0f, 1.0f);
	}else
	{
		const String & color = node->AsString();
		
		int r = HexCharToInt(color[0]) * 16 + HexCharToInt(color[1]);
		int g = HexCharToInt(color[2]) * 16 + HexCharToInt(color[3]);
		int b = HexCharToInt(color[4]) * 16 + HexCharToInt(color[5]);
		int a = HexCharToInt(color[6]) * 16 + HexCharToInt(color[7]);
		
		return Color((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
	}
}
	

Font * UIYamlLoader::GetFontByName(const String & fontName)
{
	Map<String, Font*>::iterator it = fontMap.find(fontName);
	if (it != fontMap.end())
	{
		Font * font = it->second;
		return font;
	}
	return 0;
}

void UIYamlLoader::Load(UIControl * rootControl, const String & yamlPathname)
{
	UIYamlLoader * loader = new UIYamlLoader();
	
	loader->ProcessLoad(rootControl, yamlPathname);

	loader->Release();
}

void UIYamlLoader::ProcessLoad(UIControl * rootControl, const String & yamlPathname)
{
	uint64 t1 = SystemTimer::Instance()->AbsoluteMS();
	YamlParser * parser = YamlParser::Create(yamlPathname);
	if (!parser)
	{
		Logger::Error("Failed to open yaml file: %s", yamlPathname.c_str());
		return;
	}
	
	YamlNode * rootNode = parser->GetRootNode();
	
	for (Map<String, YamlNode*>::iterator t = rootNode->AsMap().begin(); t != rootNode->AsMap().end(); ++t)
	{
		YamlNode * node = t->second;
		YamlNode * typeNode = node->Get("type");
		if (!typeNode)continue;
		
		const String & type = typeNode->AsString();
		if (type == "FTFont")
		{
			// parse font
			YamlNode * fontNameNode = node->Get("name");
			if (!fontNameNode)continue;
			
			float32 fontSize = 10.0f;
			YamlNode * fontSizeNode = node->Get("size");
			if (fontSizeNode)fontSize = fontSizeNode->AsFloat();
			
			FTFont * font = FTFont::Create(fontNameNode->AsString());
			font->SetSize(fontSize);
			
			
			YamlNode * fontColorNode = node->Get("color");
			if (fontColorNode)
			{
				Color color = GetColorFromYamlNode(fontColorNode);
				font->SetColor(color);
			}
			
			YamlNode * fontShadowColorNode = node->Get("shadowColor");
			if (fontShadowColorNode)
			{
				Color color = GetColorFromYamlNode(fontShadowColorNode);
				font->SetShadowColor(color);
			}
            
            YamlNode * fontVerticalSpacingNode = node->Get("verticalSpacing");
            if(fontVerticalSpacingNode)
            {
                font->SetVerticalSpacing(fontVerticalSpacingNode->AsInt());
            }
            
			fontMap[t->first] = font;
		}
		else if(type == "GraphicsFont")
		{
			// parse font
			YamlNode * fontNameNode = node->Get("sprite");
			if (!fontNameNode)continue;

			YamlNode * definitionNode = node->Get("definition");
			if (!definitionNode)continue;

			GraphicsFont * font = GraphicsFont::Create(definitionNode->AsString(), fontNameNode->AsString());

			YamlNode * fontSizeNode = node->Get("size");
			if (fontSizeNode)
			{
				font->SetSize(fontSizeNode->AsFloat());
			}
            
            YamlNode * fontColorNode = node->Get("color");
			if (fontColorNode)
			{
				Color color = GetColorFromYamlNode(fontColorNode);
				font->SetColor(color);
			}
            else
            {
                font->SetColor(1.f, 1.f, 1.f, 1.f);
            }
            
            YamlNode * fontVerticalSpacingNode = node->Get("verticalSpacing");
            if(fontVerticalSpacingNode)
            {
                font->SetVerticalSpacing(fontVerticalSpacingNode->AsInt());
            }

			fontMap[t->first] = font;
		}
	}
	
	LoadFromNode(rootControl, rootNode, false);
	SafeRelease(parser);
	
	for (Map<String, Font *>::iterator t = fontMap.begin(); t != fontMap.end(); ++t)
	{
		Font * font = t->second;
		SafeRelease(font);
	}
	fontMap.clear();
	uint64 t2 = SystemTimer::Instance()->AbsoluteMS();
	Logger::Debug("Load of %s time: %lld", yamlPathname.c_str(), t2 - t1);
}
	
void UIYamlLoader::LoadFromNode(UIControl * parentControl, YamlNode * rootNode, bool needParentCallback)
{
	//for (Map<String, YamlNode*>::iterator t = rootNode->AsMap().begin(); t != rootNode->AsMap().end(); ++t)
	int cnt = rootNode->GetCount();
	for (int k = 0; k < cnt; ++k)
	{
		YamlNode * node = rootNode->Get(k);
		YamlNode * typeNode = node->Get("type");
		if (!typeNode)continue;
		const String & type = typeNode->AsString();
		if (type == "FTFont")continue;
		if (type == "GraphicsFont")continue;
		
		UIControl * control = dynamic_cast<UIControl*> (ObjectFactory::Instance()->New(type));
		if (!control)
		{
			Logger::Debug("ObjectFactory haven't found object with name:%s", type.c_str());
			continue;
		}else
		{
			//Logger::Debug("Create control with type:%s", type.c_str());
		}
		control->LoadFromYamlNode(node, this);
		parentControl->AddControl(control);
		LoadFromNode(control, node, true);
		control->SetName(rootNode->GetItemKeyName(k));
		SafeRelease(control);
	}
    
    if(needParentCallback)
	{
        parentControl->LoadFromYamlNodeCompleted();   
    }
}
}