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
#ifndef __DAVAENGINE_RESOURCE_H__
#define __DAVAENGINE_RESOURCE_H__

#include "Base/BaseObject.h"

namespace DAVA
{

/**
	\ingroup render
	\brief Base class for render system resources. All render system resources must be derived from this class. 
 */
class RenderResource : public BaseObject
{
public:
	RenderResource();
	virtual ~RenderResource();
	
	/**
		\brief 	This function is called when you need to save resource to system memory for easy restore later. 
				This function should be overloaded for resouces that do not regenerating every frame, like rendertargets. 
				So if application lost focus framework call this function to save resources to system memory for each restore when device will be lost
	*/
	virtual void SaveToSystemMemory();


	/**
		\brief 	Resource that lost send this message to release old unused data and prepare to be reloaded
				This function must be overloaded in all resources that can be lost and it should free all data used by resource and prepare itself for reload
	*/
	virtual void Lost();
	
	/**
		\brief 	Invalidate resource data. Restore it from original location if needed.
	*/
	virtual void Invalidate();


	static List<RenderResource*> resourceList;
	static void LostAllResources();
	static void InvalidateAllResources();
	static void SaveAllResourcesToSystemMem();
};

};

#endif // __DAVAENGINE_RESOURCE_H__