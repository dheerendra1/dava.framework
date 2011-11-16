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
#ifndef __DAVAENGINE_YAML_DOM_PARSER_H__
#define __DAVAENGINE_YAML_DOM_PARSER_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Base/BaseMath.h"
#include <stack>

namespace DAVA 
{
class YamlParser;

/**
	\defgroup yaml Yaml configs
 */
/** 
	\ingroup yaml
	\brief this class is base yaml node that is used for everything connected with yaml
*/
class YamlNode : public BaseObject
{
public:
	enum eType
	{
		TYPE_STRING = 0,
		TYPE_ARRAY,
		TYPE_MAP,
	};

	YamlNode(eType type);
	virtual ~YamlNode();
	
	void Print(int32 identation);
	
	bool			AsBool();
	int32			AsInt();
	float32			AsFloat();
	const String &	AsString();
	const WideString & AsWString();
	Vector<YamlNode*> & AsVector();
	Map<String, YamlNode*> & AsMap();
	
	/*
		These functions work only if type of node is array
		All values must be integer or float to perform this conversion
	 */
	Vector2			AsPoint();//Dizz: this one exists cause of Boroda
	Vector2			AsVector2();
	Vector3			AsVector3();
	Rect			AsRect();	
	
	YamlNode *		Get(const String & name);
	YamlNode *		Get(int32 index); 
	const String &	GetItemKeyName(int32 index); 
	
	eType			GetType() { return type; }
	int32			GetCount();
	
private:
	int						mapIndex;
	int						mapCount;
	eType					 type;
	WideString				stringValue;
	String					 nwStringValue;
	Vector<YamlNode*>		 objectArray;
	Map<String, YamlNode*>	objectMap;	

	friend class YamlParser;
};

/** 
	\ingroup yaml
	\brief this class is yaml parser and it used if you want to parse yaml file
 */
class YamlParser : public BaseObject
{
protected:
	YamlParser();
	virtual ~YamlParser();
	bool Parse(const String & fileName);
	
public:
	static YamlParser	* Create(const String & fileName);
	
	YamlNode			* GetRootNode();
	
	struct YamlDataHolder
	{
		uint32 fileSize;
		uint32 dataOffset;
		uint8 * data;
	};

private:
	YamlNode			* GetNodeByPath(const String & path);
	
	YamlNode			* rootObject;
	String				lastMapKey;
	
	std::stack<YamlNode *> objectStack;

	YamlDataHolder dataHolder;
};

};

#endif // __DAVAENGINE_JSON_PARSER_H__