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
#ifndef __DAVAENGINE_MESSAGE__
#define __DAVAENGINE_MESSAGE__

#include "Base/BaseObject.h"

namespace DAVA 
{

	

class MessageBase : public BaseObject
{
public:
	virtual ~MessageBase() {};
	virtual void operator () (BaseObject *, void *, void *) = 0;
	virtual MessageBase * Clone() = 0;
	virtual bool IsEqual(MessageBase * message) = 0;

};

template<class T>
class MessageBaseClassFunctionImpl : public MessageBase
{
	T * targetObject;
	void (T::*targetFunction)(BaseObject *, void *, void *);
public:
	MessageBaseClassFunctionImpl(T *_object, void (T::*_function)(BaseObject *, void *, void *)) 
	{
		targetObject = _object;
		targetFunction = _function;
	}
	
	virtual void operator () (BaseObject * callerObject, void * userData, void * callerData)
	{
		(targetObject->*targetFunction)(callerObject, userData, callerData);
	}
	
	virtual MessageBase * Clone()
	{
		return new MessageBaseClassFunctionImpl(targetObject, targetFunction);
	}
	
	virtual bool IsEqual(MessageBase * messageBase)
	{
		MessageBaseClassFunctionImpl<T> * t = dynamic_cast<MessageBaseClassFunctionImpl<T>*> (messageBase);
		if (t != 0)
		{
			if (targetObject == t->targetObject && targetFunction == t->targetFunction)return true;
		}
		return false;
	}
};
	
class MessageBaseStaticFunctionImpl : public MessageBase
{
	void (*targetFunction)(BaseObject *, void *, void *);
public:
	MessageBaseStaticFunctionImpl(void (*_function)(BaseObject *, void *, void *)) 
	{
		targetFunction = _function;
	}
	
	virtual void operator () (BaseObject * callerObject, void * userData, void * callerData)
	{
		(*targetFunction)(callerObject, userData, callerData);
	}
	
	virtual MessageBase * Clone()
	{
		return new MessageBaseStaticFunctionImpl(targetFunction);
	}
	
	virtual bool IsEqual(MessageBase * messageBase)
	{
		MessageBaseStaticFunctionImpl * t = dynamic_cast<MessageBaseStaticFunctionImpl*> (messageBase);
		if (t != 0)
		{
			if (targetFunction == t->targetFunction)return true;
		}
		return false;
	}
};

	
/**
	\ingroup baseobjects
	\brief	Message class defines a wrapper to store both pointer to class and it's function to call it easily. 
			All SDK subsystems use messages for event handling. 
 
	This code assign a message Message(this, &MenuScreen::ButtonPressed) to button event. 
 	\code
	playButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &MenuScreen::ButtonPressed));
	\endcode
	This means that if event will be performed function ButtonPressed from MenuScreen 
	class with pointer to this will be called.
	This method is normally used together with \ref EventDispatcher class.
 */
	
class Message
{
	MessageBase * messageBase;
	void * userData;
public:
	
	template<class C>
	Message(C * targetObject, void (C::*targetFunction)(BaseObject*, void*, void*), void * _userData = 0)
	{
		userData = _userData;
		messageBase = new MessageBaseClassFunctionImpl<C>(targetObject, targetFunction);
	}	

	Message(void (*targetFunction)(BaseObject*, void*, void*), void * _userData = 0)
	{
		userData = _userData;
		messageBase = new MessageBaseStaticFunctionImpl(targetFunction);
	}	
	
	Message()
	:	messageBase(0)
	,	userData(0)
	{
	}
	
	~Message()
	{
		SafeRelease(messageBase);
	}
	
	// void SetSelector(BaseObject *_pObj, void (BaseObject::*_pFunc)(BaseObject*, void*, void*), void * _pUserData);

	void operator() (BaseObject * caller)
	{	
		if (messageBase) 
		{
			(*messageBase)(caller, userData, 0);
		}
	}

	void operator() (BaseObject * caller, void * callerData)
	{	
		if (messageBase) 
		{
			(*messageBase)(caller, userData, callerData);
		}
	}
	
	bool operator == (const Message &msg) const
	{
		return (messageBase->IsEqual(msg.messageBase));
	}

	Message(const Message & msg)
	{
		messageBase = SafeRetain(msg.messageBase);
		userData = msg.userData;
	}
	
	Message & operator =(const Message & msg)
	{
		messageBase = SafeRetain(msg.messageBase);
		userData = msg.userData;
		return *this;
	}
	
	void SetUserData(void * newData)
	{
		userData = newData;
	}
    
    bool IsEmpty()
    {
        return (messageBase==0);
    }

};
};

#endif