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
#include "DAVAConfig.h"
#include "Base/BaseObject.h"
#include "Base/BaseObjectChecker.h"
#include "Debug/List.h"
#include "FileSystem/Logger.h"

#ifdef ENABLE_BASE_OBJECT_CHECKS

#include <set>



namespace DAVA 
{

std::set<BaseObject*> baseObjects;

void BaseObjectChecker::RegisterBaseObject(BaseObject * obj)
{
	baseObjects.insert(obj);
}

	
void BaseObjectChecker::UnregisterBaseObject(BaseObject * obj)
{
	std::set<BaseObject*>::iterator r = baseObjects.find(obj);
	if (r != baseObjects.end())
	{
		baseObjects.erase(r);
	}
}

bool BaseObjectChecker::IsAvailable(BaseObject * obj)
{
	std::set<BaseObject*>::iterator r = baseObjects.find(obj);
	if (r != baseObjects.end())
	{
		return true;
	}
	return false;
}

void BaseObjectChecker::Dump()
{
    for (std::set<BaseObject*>::iterator it = baseObjects.begin(); it != baseObjects.end(); ++it)
    {
        BaseObject * obj = *it;
        Logger::Debug("(%s) object not released", typeid(obj).name());
    }
}

}

#endif // ENABLE_BASE_OBJECT_CHECKS

