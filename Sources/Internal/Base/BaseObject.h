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
#ifndef __DAVAENGINE_BASEOBJECT_H__
#define __DAVAENGINE_BASEOBJECT_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObjectChecker.h"
#include "Debug/DVAssert.h"
#include "DAVAConfig.h"
#include "Base/RefPtr.h"
#include "Render/RenderBase.h"

namespace DAVA
{

	
/**
	\defgroup baseobjects Framework Base Objects
	This group contain all framework classes which defines the basics of our system. 
	All architecture approaches & patterns we've selected in our framework is well-known and described. 
 */
/** 
	\ingroup baseobjects
	\brief class to implement object reference counting
 
	This class is parent for most of classes in our SDK. 
	When you create it reference counter equal to 1. 
  */

	
class	BaseObject
{
public:
	
	//! Constructor
	BaseObject()
		: referenceCount(1)
	{
#ifdef ENABLE_BASE_OBJECT_CHECKS
		BaseObjectChecker::RegisterBaseObject(this);
#endif 
	}

	//! Destructor
	virtual ~BaseObject()
	{
#ifdef ENABLE_BASE_OBJECT_CHECKS
		BaseObjectChecker::UnregisterBaseObject(this);
#endif 
	}

	/**
		\brief Increment reference counter in this object.
	 */
	virtual void Retain()
	{
		++referenceCount;
	}
	
	/** 
		\brief Decrement object reference counter and delete it if reference counter equal to 0.
		\returns referenceCounter value after decrement
	 */
	virtual int32 Release()
	{
#ifdef ENABLE_BASE_OBJECT_CHECKS
		if (!BaseObjectChecker::IsAvailable(this))
		{
			DVASSERT(0 && "Attempt to delete unavailable BaseObject");
		}	
#endif		

		--referenceCount;
		int32 refCounter = referenceCount;
		if (!refCounter)
		{
			delete this;
		}
		return refCounter;
	}

	/** 
		\brief return current number of references for this object
		\returns referenceCounter value 
	 */
	int32 GetRetainCount() const
	{
		return referenceCount;
	}
protected:
	
	BaseObject(const BaseObject & b)
	{
	}
	BaseObject & operator = (const BaseObject & b)
	{
		return *this;
	}
	
	int32 referenceCount;
};


/** 
	\ingroup baseobjects
	\brief	function to perform release safely. It checks if given object not equal to zero, in debug mode it also checks if such object 
			haven't deallocated before and only if both checks is positive it call Release. After release it set value of variable to 0 to avoid 
			possible errors with usage of this variable
 */
template<class C>
void SafeRelease(C * &c) 
{ 
	if (c) 
	{
#ifdef ENABLE_BASE_OBJECT_CHECKS
		if (!BaseObjectChecker::IsAvailable(c))
		{
			DVASSERT(0 &&"SafeRelease Attempt to access unavailable BaseObject");
		}
#endif
		c->Release();
		c = 0;
	}
}

// /*#if defined(__DAVAENGINE_DIRECTX9__)*/
//template<>
//inline void SafeRelease<IUnknown>(IUnknown * &c) 
//{ 
//	if (c) 
//	{
//		c->Release();
//		c = 0;
//	}
//}
// // #endif 
/** 
	\ingroup baseobjects
	\brief	function to perform retain safely. Only if object exists it perform retain.
	\return same object with incremented reference count
*/
template<class C>
C * SafeRetain(C * c) 
{ 
	if (c) 
	{
#ifdef ENABLE_BASE_OBJECT_CHECKS
		BaseObject * c2 = dynamic_cast<BaseObject*>(c);
		if(c2)
		{
			if (!BaseObjectChecker::IsAvailable(c))
			{
				DVASSERT(0 &&"RetainedObject Attempt to access unavailable BaseObject");
			}
		}
#endif
		c->Retain();
	}
	return c;
}
	
/*template<class C>
C * SafeClone(C * object)
{
	if (object)
		return object->Clone();
	return 0;
}
	
// Boroda: Do not work when it's here, only works when it embedded into cpp file. WTF?
template<typename C>
RefPtr<C> SafeClone(const RefPtr<C> & object)
{
	if (object)
		return RefPtr<C>(object->Clone());
	return RefPtr<C>(0);
}
*/
}; 

/*
 For BaseObject* code is wrong / saved for potential use in other class
 
 #ifdef ENABLE_BASE_OBJECT_CHECKS
 virtual BaseObject& operator*() const throw() 
 {
 if (!BaseObjectChecker::IsAvailable((BaseObject*)this))
 {
 DVASSERT(0 &&"Attempt to access unavailable BaseObject");
 }	
 return *(BaseObject*)this; 
 }
 virtual BaseObject* operator->() const throw() 
 {
 if (!BaseObjectChecker::IsAvailable((BaseObject*)this))
 {
 DVASSERT(0 &&"Attempt to access unavailable BaseObject");
 }	
 return (BaseObject*)this; 
 }
 #endif 
 */


/*	
 Мысли на тему умного учета объектов и сериализации
 
 
 #define IMPLEMENT_CLASS(name)
 String GetName()
 {
 return String(#name);
 }	
 
 int		GetID()
 {
 return GlobalIdCounter++;
 }

 class A : public BaseObject
 {
 IMPLEMENT_CLASS(A);
 
 
 };
 
 */



#endif // __DAVAENGINE_BASEOBJECT_H__

