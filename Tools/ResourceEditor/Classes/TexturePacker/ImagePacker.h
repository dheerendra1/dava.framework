/*======== (C) Copyright 1999 LogGames, L.L.C. All rights reserved. ==========

 The copyright to the contents herein is the property of LogGames, L.L.C.
 The contents may be used and/or copied only with the written permission of
 LogGames, L.L.C., or in accordance with the terms and conditions stipulated in
 the agreement/contract under which the contents have been supplied.

 Revision History
	* Created by Borodovsky Vitaliy
=============================================================================*/
#ifndef __DAVAENGINE_IMAGEPACKER_H__
#define __DAVAENGINE_IMAGEPACKER_H__

#include "DAVAEngine.h"

using namespace DAVA;

//! helper class to simplify packing of many small 2D images to one big 2D image
class ImagePacker
{
public:
	//! \brief constructor
	//! \param[in] size of this imagePacker
	ImagePacker(const Rect2i & _rect);
	
	//! \brief destructor
	virtual ~ImagePacker();

	//! \brief release all data allocated by image packer and reset it internal state
	void Release(); 
	
	//! \brief Add image to image packer & image packer must allocate position for this image
	//! \param[in] imageSize image size of image we want to pack
	//! \return rectangle with image position in current image topology
	bool AddImage(const Size2i & imageSize, void * searchPtr);
	Rect2i * SearchRectForPtr(void * searchPtr);
	
	Rect2i & GetRect() { return rect; };
private:
	// Implementation details
	Rect2i rect;
	
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
		Rect2i			rect;
		bool			isLeaf;
		PackNode *		child[2];
		void *			searchPtr;

		PackNode *	Insert(const Size2i & imageSize);
		Rect2i * SearchRectForPtr(void * searchPtr);
		void		Release();
	};
	
	
	PackNode * root;
};

#endif // __DAVAENGINE_IMAGEPACKER_H__
