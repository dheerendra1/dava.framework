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
#include "Particles/ParticlePropertyLine.h"


namespace DAVA
{

RefPtr<PropertyLine<float32> > PropertyLineYamlReader::CreateFloatPropertyLineFromYamlNode(YamlNode * parentNode, const String & propertyName, RefPtr<PropertyLine<float32> > defaultPropertyLine)
{
	YamlNode * node = parentNode->Get(propertyName);
	if (!node)return defaultPropertyLine;

	if (node->GetType() == YamlNode::TYPE_STRING)
	{
		return RefPtr< PropertyLine<float32> >(new PropertyLineValue<float32>(node->AsFloat()));
	}else if (node->GetType() == YamlNode::TYPE_ARRAY)
	{
		RefPtr<PropertyLineKeyframes<float32> > keyframes(new PropertyLineKeyframes<float32>());

		for (int k = 0; k < node->GetCount() / 2; ++k)
		{
			YamlNode * time = node->Get(k * 2);
			YamlNode * value = node->Get(k * 2 + 1);

			if (time && value)
			{
				keyframes->AddValue(time->AsFloat(), value->AsFloat());
			}
		}
		return keyframes;
	}
	return RefPtr<PropertyLine<float32> >();
}

RefPtr< PropertyLine<Vector2> > PropertyLineYamlReader::CreateVector2PropertyLineFromYamlNode( YamlNode * parentNode, const String & propertyName, RefPtr< PropertyLine<Vector2> > defaultPropertyLine /*= 0*/ )
{
	YamlNode * node = parentNode->Get(propertyName);
	if (!node)return defaultPropertyLine;

	if (node->GetType() == YamlNode::TYPE_STRING)
	{
		float32 v = node->AsFloat();
		return RefPtr< PropertyLine<Vector2> >(new PropertyLineValue<Vector2>(Vector2(v, v)));
	}else if (node->GetType() == YamlNode::TYPE_ARRAY)
	{
		if (node->GetCount() == 2)
		{
			Vector2 res(1.0f, 1.0f);
			res = node->AsPoint();
			return RefPtr< PropertyLine<Vector2> >(new PropertyLineValue<Vector2>(res));
		}

		RefPtr< PropertyLineKeyframes<Vector2> > keyframes (new PropertyLineKeyframes<Vector2>());

		for (int k = 0; k < node->GetCount() / 2; ++k)
		{
			YamlNode * time = node->Get(k * 2);
			YamlNode * value = node->Get(k * 2 + 1);

			if (time && value)
			{
				if (value->GetType() == YamlNode::TYPE_ARRAY)
				{
					keyframes->AddValue(time->AsFloat(), value->AsPoint());
				}
				else 
				{
					float32 v = value->AsFloat();
					keyframes->AddValue(time->AsFloat(), Vector2(v, v));
				}
			}
		}
		return keyframes;
	}

	return RefPtr< PropertyLine<Vector2> >();
}

Color ColorFromYamlNode(YamlNode * node)
{
	Color c;
	c.r = node->Get(0)->AsFloat() / 255.0f;
	c.g = node->Get(1)->AsFloat() / 255.0f;
	c.b = node->Get(2)->AsFloat() / 255.0f;
	c.a = node->Get(3)->AsFloat() / 255.0f;
	return c;
}

RefPtr< PropertyLine<Color> > PropertyLineYamlReader::CreateColorPropertyLineFromYamlNode( YamlNode * parentNode, const String & propertyName, RefPtr< PropertyLine<Color> > defaultPropertyLine)
{
	YamlNode * node = parentNode->Get(propertyName);
	if (!node)return defaultPropertyLine;

	if (node->GetType() == YamlNode::TYPE_ARRAY)
	{
		bool allString = true;
		for (int k = 0; k < node->GetCount(); ++k)
			if (node->Get(k)->GetType() != YamlNode::TYPE_STRING)
				allString = false;

		if (allString && node->GetCount() == 4)
		{
			return RefPtr< PropertyLine<Color> >(new PropertyLineValue<Color>(ColorFromYamlNode(node)));
		}else
		{
			RefPtr< PropertyLineKeyframes<Color> > keyframes (new PropertyLineKeyframes<Color>());

			for (int k = 0; k < node->GetCount() / 2; ++k)
			{
				YamlNode * time = node->Get(k * 2);
				YamlNode * value = node->Get(k * 2 + 1);

				if (time && value)
				{
					if (value->GetType() == YamlNode::TYPE_ARRAY)
					{
						keyframes->AddValue(time->AsFloat(), ColorFromYamlNode(value));
					}
				}
			}
			return keyframes;
		}
	}
	return RefPtr< PropertyLine<Color> >();
}

}