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
#ifndef __LOGENGINE_IMAGEPACKER_H__
#define __LOGENGINE_IMAGEPACKER_H__

#include "BaseTypes.h"
#include "BaseMath.h"

namespace Log
{
namespace Render
{

//! helper class to simplify packing of many small 2D images to one big 2D image
class ImagePacker
{
public:
	//! \brief constructor
	//! \param[in] size of this imagePacker
	ImagePacker(const Math::Rect2i & _rect);
	
	//! \brief destructor
	virtual ~ImagePacker();

	//! \brief release all data allocated by image packer and reset it internal state
	void Release(); 
	
	//! \brief Add image to image packer & image packer must allocate position for this image
	//! \param[in] imageSize image size of image we want to pack
	//! \return rectangle with image position in current image topology
	bool AddImage(const Math::Size2i & imageSize, void * searchPtr);
	Math::Rect2i * SearchRectForPtr(void * searchPtr);
	
	Math::Rect2i & GetRect() { return rect; };
private:
	// Implementation details
	Math::Rect2i rect;
	
	struct PackNode
	{
		PackNode()
		{
			isLeaf = true;
			child[0] = 0;
			child[1] = 0;
			isImageSet = false;
			searchPtr = 0;
		}
	
		bool			isImageSet;
		Math::Rect2i	rect;
		bool			isLeaf;
		PackNode *		child[2];
		void *			searchPtr;

		PackNode *	Insert(const Math::Size2i & imageSize);
		Math::Rect2i * SearchRectForPtr(void * searchPtr);
		void		Release();
	};
	
	
	PackNode * root;
};

};
};
using namespace Log;
using namespace Render;

#endif // __LOGENGINE_IMAGEPACKER_H__
