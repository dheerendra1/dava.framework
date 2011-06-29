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
#ifndef __DAVAENGINE_OBJECT_FACTORY_H__
#define __DAVAENGINE_OBJECT_FACTORY_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Base/StaticSingleton.h"
#include "Base/ObjectCreator.h"

namespace DAVA 
{

typedef BaseObject* (*CreateObjectFunc)();

class ObjectRegistrator
{
public:
	ObjectRegistrator(const String & name, CreateObjectFunc func);
};
	
#define REGISTER_CLASS(class_name) \
static BaseObject * Create##class_name()\
{\
	return new class_name();\
};\
static ObjectRegistrator registrator##class_name(#class_name, &Create##class_name);

	
#define REGISTER_CLASS_WITH_STRING_NAME(class_name, symbol_name) \
static BaseObject * Create##class_name()\
{\
return new class_name();\
};\
static ObjectRegistrator registrator##class_name(symbol_name, &Create##class_name);
	

/**
	\ingroup baseobjects
	\brief	This class allow to create classes by name
 
	For example you want to create bunch of classes using their names it can be done easily using framework
	In implementation of your class you should write REGISTER_CLASS(ClassName) and this class now can 
	be created by it's name using ObjectFactory singleton
	
	Class must be inherited from BaseObject and have default constructor.
 
	Example:
	
	Definition of class
	\code
	// UIButton.h
	class UIButton : public BaseObject
	{
		UIButton::UIButton()
 
	};
	\endcode
	
	Implementation of class
	\code
	// UIButton.cpp
	
	REGISTER_CLASS(UIButton);
	
	UIButton::UIButton()
	{
	}
	\endcode
		
	Usage of ObjectFactory:
	\code
		
	void MyFunction()
	{
		UIButton * button = dynamic_cast<UIButton*>(ObjectFactory::Instance()->New("UIButton"));
		
	}
 
	\endcode 
 */

class ObjectFactory : public StaticSingleton<ObjectFactory>
{
public:
	/**
		\brief creates a class with given name
		
		\param[in] name name of class you want to create
	 */
	BaseObject * New(const String & name); 
	
	/**
		\brief This function is supposed to RegisterObjectCreator
		
		It used internally by REGISTER_CLASS define
		\param[in] name this is name of class we want to register
		\param[in] func this is pointer to function that can create such class
	*/
	void RegisterObjectCreator(const String & name, CreateObjectFunc func);
private:
	Map<String, CreateObjectFunc> creatorMap;

};
};

#endif // __DAVAENGINE_OBJECT_FACTORY_H__