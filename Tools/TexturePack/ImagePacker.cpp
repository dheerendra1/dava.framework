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
#include "ImagePacker.h"
#include "BaseTypes.h"

namespace Log
{
namespace Render
{

ImagePacker::PackNode * ImagePacker::PackNode::Insert(const Math::Size2i & imageSize)
{
	if (!isLeaf)
	{
		ImagePacker::PackNode *newNode = child[0]->Insert(imageSize);
		if (newNode)return newNode;

		return child[1]->Insert(imageSize);
	}else
	{
		if (isImageSet)return 0;


		if ((imageSize.dx > rect.dx) || (imageSize.dy > rect.dy))
		{
			return 0;
		}
		if ((imageSize.dx == rect.dx) && (imageSize.dy == rect.dy))
		{
			isImageSet = true;
			return this;
		}

		isLeaf = false;

		child[0] = new ImagePacker::PackNode;
		child[1] = new ImagePacker::PackNode;

		int32 dw = rect.dx - imageSize.dx;
		int32 dh = rect.dy - imageSize.dy;

		if (dw > dh)
		{
			child[0]->rect = Rect2i::Make(	rect.x, rect.y, imageSize.dx, rect.dy);
			child[1]->rect = Rect2i::Make(	rect.x + imageSize.dx, rect.y, rect.dx - imageSize.dx, rect.dy);
		}else
		{
			child[0]->rect = Rect2i::Make(	rect.x, rect.y, rect.dx, imageSize.dy);
			child[1]->rect = Rect2i::Make(	rect.x, rect.y + imageSize.dy, rect.dx, rect.dy - imageSize.dy);
		}
		return child[0]->Insert(imageSize);
	}
}

void ImagePacker::PackNode::Release()
{
	if (child[0])
		child[0]->Release();

	if (child[1])
		child[1]->Release();

	delete this;
}


ImagePacker::ImagePacker(const Math::Rect2i & _rect)
{
	root = new PackNode;
	root->rect = _rect;
	rect = _rect;
}

ImagePacker::~ImagePacker()
{
	Release();
}

void ImagePacker::Release()
{
	if (root);
		root->Release();
}

bool ImagePacker::AddImage(const Math::Size2i & imageSize, void * searchPtr)
{
	PackNode * node = root->Insert(imageSize);
	if (node)
	{
		node->searchPtr = searchPtr;
		printf("set search ptr to rect:(%d, %d) ims: (%d, %d): 0x%08x\n", node->rect.dx, node->rect.dy, imageSize.dx, imageSize.dy, searchPtr);
	}	
	return (node != 0);
}
	
Math::Rect2i * ImagePacker::SearchRectForPtr(void * searchPtr)
{
	return root->SearchRectForPtr(searchPtr);
}

Math::Rect2i * ImagePacker::PackNode::SearchRectForPtr(void * searchPtr)
{
	if (searchPtr == this->searchPtr)
	{
		return &this->rect;
	}
	else {
		Math::Rect2i * resultRect = 0;
		if (child[0])resultRect = child[0]->SearchRectForPtr(searchPtr);
		if (child[1] && (!resultRect))resultRect = child[1]->SearchRectForPtr(searchPtr);
		
		return resultRect;
	}
}
	
};
};
