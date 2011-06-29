#include "ImagePacker.h"
#include "Base/BaseTypes.h"
#include "CommandLineParser.h"


ImagePacker::PackNode * ImagePacker::PackNode::Insert(const Size2i & imageSize)
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
			child[0]->rect = Rect2i(	rect.x, rect.y, imageSize.dx, rect.dy);
			child[1]->rect = Rect2i(	rect.x + imageSize.dx, rect.y, rect.dx - imageSize.dx, rect.dy);
		}else
		{
			child[0]->rect = Rect2i(	rect.x, rect.y, rect.dx, imageSize.dy);
			child[1]->rect = Rect2i(	rect.x, rect.y + imageSize.dy, rect.dx, rect.dy - imageSize.dy);
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


ImagePacker::ImagePacker(const Rect2i & _rect)
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
	if (root)
	{
		root->Release();
		root = 0;
	}
}

bool ImagePacker::AddImage(const Size2i & imageSize, void * searchPtr)
{
	PackNode * node = root->Insert(imageSize);
	if (node)
	{
		node->searchPtr = searchPtr;
		if (CommandLineParser::Instance()->GetVerbose())
			printf("set search ptr to rect:(%d, %d) ims: (%d, %d)\n", node->rect.dx, node->rect.dy, imageSize.dx, imageSize.dy);
	//		printf("set search ptr to rect:(%d, %d) ims: (%d, %d): 0x%08x\n", node->rect.dx, node->rect.dy, imageSize.dx, imageSize.dy, searchPtr);
	}	
	return (node != 0);
}
	
Rect2i * ImagePacker::SearchRectForPtr(void * searchPtr)
{
	return root->SearchRectForPtr(searchPtr);
}

Rect2i * ImagePacker::PackNode::SearchRectForPtr(void * searchPtr)
{
	if (searchPtr == this->searchPtr)
	{
		return &this->rect;
	}
	else {
		Rect2i * resultRect = 0;
		if (child[0])resultRect = child[0]->SearchRectForPtr(searchPtr);
		if (child[1] && (!resultRect))resultRect = child[1]->SearchRectForPtr(searchPtr);
		
		return resultRect;
	}
}
	