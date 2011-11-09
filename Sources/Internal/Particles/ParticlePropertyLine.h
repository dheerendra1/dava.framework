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
#ifndef __DAVAENGINE_PARTICLES_PROPERTY_LINE_H__
#define __DAVAENGINE_PARTICLES_PROPERTY_LINE_H__

#include "FileSystem/YamlParser.h"
#include "Base/RefPtr.h"

namespace DAVA
{

template<class T>
class PropertyLine : public BaseObject
{
public:
	virtual const T & GetValue(float32 t) = 0;
	virtual PropertyLine<T>* Clone() {return 0;};

};

class PropertyLineYamlReader
{
public:
	static RefPtr<PropertyLine<float32> > CreateFloatPropertyLineFromYamlNode(YamlNode * parentNode, const String & propertyName, RefPtr<PropertyLine<float32> > defaultPropertyLine = RefPtr< PropertyLine<float32> >());
	static RefPtr<PropertyLine<Vector2> > CreateVector2PropertyLineFromYamlNode(YamlNode * parentNode, const String & propertyName, RefPtr<PropertyLine<Vector2> > defaultPropertyLine = RefPtr< PropertyLine<Vector2> >());
	static RefPtr<PropertyLine<Color> >  CreateColorPropertyLineFromYamlNode(YamlNode * parentNode, const String & propertyName, RefPtr<PropertyLine<Color> > defaultPropertyLine = RefPtr< PropertyLine<Color> >());
};

/*#define PROPERTY_RETAIN(type, name)\
private:\
type property##name;\
public:\
void Set##name(type _v)\
{\
	SafeRelease(property##name);\
	property##name = SafeRetain(_v);\
}\
type Get##name(){return property##name;};
*/


template<class T>
class PropertyLineValue : public PropertyLine<T>
{
public:
	T value;
	PropertyLineValue(T _value)
	{
		value = _value;
	}	

	const T & GetValue(float32 t) { return value; }
	PropertyLine<T>* Clone()
	{	
		if (this == 0)return 0;
		return new PropertyLineValue<T>(value); 
	}	
};

template<class T>
class PropertyLineKeyframes : public PropertyLine<T>
{
public:
	struct PropertyKey
	{
		float32 t;
		T value;
	};
	std::vector<PropertyKey> 	keys;
	T resultValue;
	
	const T & GetValue(float32 t) 
	{
		int32 keysSize = (int32)keys.size();
		if(t > keys[keysSize - 1].t)
		{
			return keys[keysSize - 1].value;
		}
		if (keys.size() == 2)
		{
			if (t < keys[1].t)
			{
				float ti = (t - keys[0].t) / (keys[1].t - keys[0].t);
				resultValue = keys[0].value + (keys[1].value - keys[0].value) * ti;
				return resultValue;
			}else 
			{	
			}
		}
		else if (keys.size() == 1) return keys[0].value; 
		else
		{
			int32 l = BinaryFind(t, 0, (int32)keys.size() - 1);

			float ti = (t - keys[l].t) / (keys[l + 1].t - keys[l].t);
			resultValue = keys[l].value + (keys[l + 1].value - keys[l].value) * ti;
		}
		return resultValue;
	}

	int32 BinaryFind(float32 t, int32 l, int32 r)
	{
		if (l + 1 == r)	// we've found a solution
		{
			return l;
		}

		int32 m = (l + r) >> 1; //l + (r - l) / 2 = l + r / 2 - l / 2 = (l + r) / 2; 
		if (t <= keys[m].t)return BinaryFind(t, l, m);
		else return BinaryFind(t, m, r);
	}

	void AddValue(float32 t, T value)
	{
		PropertyKey key;
		key.t = t;
		key.value = value;
		keys.push_back(key);
	}
	
	PropertyLine<T>* Clone()
	{ 
		if (this == 0)return 0;
		PropertyLineKeyframes<T>* clone =  new PropertyLineKeyframes<T>();
		clone->keys = keys;
		return clone;
	}	

};
};




#endif // __DAVAENGINE_PARTICLES_PROPERTY_LINE_H__