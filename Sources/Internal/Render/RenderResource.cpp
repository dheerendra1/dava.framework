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
#include "Render/RenderResource.h"
#include "Render/Texture.h"

namespace DAVA
{

List<RenderResource*> RenderResource::resourceList;

RenderResource::RenderResource()
{
	resourceList.push_back(this);
}

RenderResource::~RenderResource()
{
	List<RenderResource*>::iterator it = resourceList.begin();
	for(; it != resourceList.end(); it++)
	{
		if(*it == this)
		{
			resourceList.erase(it);
			break;
		}
	}
}

void RenderResource::SaveToSystemMemory()
{

}

void RenderResource::Lost()
{
}

void RenderResource::Invalidate()
{
}

void RenderResource::LostAllResources()
{
	List<RenderResource*>::iterator it = resourceList.begin();
	for(; it != resourceList.end(); it++)
	{
		(*it)->Lost();
	}
}

void RenderResource::InvalidateAllResources()
{
	List<RenderResource*>::iterator it = resourceList.begin();
	for(; it != resourceList.end(); it++)
	{
		(*it)->Invalidate();
	}
}

void RenderResource::SaveAllResourcesToSystemMem()
{
	List<RenderResource*>::iterator it = resourceList.begin();
	for(; it != resourceList.end(); it++)
	{
		(*it)->SaveToSystemMemory();
// 		Texture * t = dynamic_cast<Texture*>((*it));
// 		if (t)
// 			Logger::Debug("%s", t->relativePathname.c_str());
	}
}




};