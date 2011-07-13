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
=====================================================================================*/
#ifndef __LOGENGINE_IBASE_H__
#define __LOGENGINE_IBASE_H__

#include "BaseTypes.h"

namespace Log
{

/** \brief class to implement object reference counting
 *
 * This class is parent for some classes in our system 
 * Our conception is very simple but we have few
 * exeptions. 
 * For Example:
 * If you create texture with CreateTexture or LoadTexture
 * You must call method Release to release them, or 
 * just decrease reference counter.
 * But if you use Global namespace you must only check 
 * if specified System not NULL. (Never call Release for
 * Globals subsystems)
 * These subsystems are managed by Engine itself
 * Remember: all methods of IEngineSystem return 
 * Global systems.
 */
class	IBase
{
public:
	
	//! Constructor
	IBase()
		: referenceCounter(1)
	{

	}

	//! Destructor
	virtual ~IBase()
	{
	}

	//! AddReference
	void AddReference()
	{
		++referenceCounter;
	}
	
	//! Decrease object Reference Count 
	//! and delete him if ReferenceCount == 0.
	int32 Release()
	{
		--referenceCounter;
		int32 refCounter = referenceCounter;
		if (!refCounter)
		{
			delete this;
		}
		return refCounter;
	}

	int32 GetRefCount() const
	{
		return referenceCounter;
	}

private:
	int32 referenceCounter;
};


template<class C>
void SafeRelease(C * &c) 
{ 
	if (c) 
	{
		c->Release();
		c = 0;
	}
}

}; // end of namespace Log

#endif // __LOGENGINE_IBASE_H__

