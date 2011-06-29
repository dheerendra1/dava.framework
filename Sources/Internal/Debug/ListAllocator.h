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
#ifndef __DAVAENGINE_LIST_ALLOCATOR_H__
#define __DAVAENGINE_LIST_ALLOCATOR_H__

#include "Base/BaseTypes.h"

namespace DAVA 
{
	
template <class ITEM>
class Allocator
{
public:
	Allocator()
	{
	}
	
	~Allocator()
	{
	}
	
	ITEM * Alloc()
	{
		void * p = malloc(sizeof(ITEM));
		//ITEM * item = new (p) ITEM;
		return (ITEM*)p;
	}
	
	void Dealloc(ITEM * node)
	{
		//node->~ITEM();
		free(node);
	}

};
	
	
/*template <class ITEM>
class Allocator
{
	struct AllocBlock
	{
		ITEM * startAddr;
		AllocBlock * nextBlock;
	};

public:
	Allocator(uint32 _reserve = 1)
	{
		reserve = _reserve;
		freeCount = 0;
		allocBlockList = NULL;
		freeNode = NULL;
	}

	~Allocator()
	{
		while (allocBlockList)
		{
			AllocBlock * temp = allocBlockList;
			allocBlockList = allocBlockList->nextBlock;
			delete[] temp->startAddr;
			delete temp;
		}
	}

	ITEM * Alloc()
	{
		if (!freeCount--)
		{
			freeCount += reserve;

			ITEM * newfreeNode = new ITEM[reserve];
			for (uint32 i = 0; i < (reserve - 1); ++i)
			{
				newfreeNode[i].nextI = &newfreeNode[i + 1];
			}

			newfreeNode[reserve - 1].nextI = freeNode;
			freeNode = newfreeNode;

			AllocBlock * newAllocBlock = new AllocBlock;
			newAllocBlock->startAddr = newfreeNode;
			newAllocBlock->nextBlock = allocBlockList;
			allocBlockList = newAllocBlock;
		}

		ITEM * node = freeNode;
		freeNode = freeNode->nextI;
		return node;
	}

	void Dealloc(ITEM * node)
	{
		node->~ITEM();
		freeCount++;
		node->nextI = freeNode;
		freeNode = node;
	}

private:

	ITEM * freeNode;
	
	uint32 freeCount;
	uint32 reserve;

	AllocBlock * allocBlockList;
};*/
	
};

#endif // __DAVAENGINE_LIST_ALLOCATOR_H__