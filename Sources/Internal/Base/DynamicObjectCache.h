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
#ifndef __DAVAENGINE_DYNAMIC_OBJECT_CACHE_H__
#define __DAVAENGINE_DYNAMIC_OBJECT_CACHE_H__

#include "Base/BaseTypes.h"
#include "Debug/DVAssert.h"

namespace DAVA
{

template <class T>
class DynamicObjectCacheData
{
public:
	DynamicObjectCacheData(int size);
	~DynamicObjectCacheData();
	
	T * New();
	void Delete(T * _item);

	void AddToCache(int size);
private:
	void CreateNewDataBlock();
	void DeallocateMemory();
	int32 size;
	List<T*> blocks;
	List<T*> cache;
};
	
template <class T>
class DynamicObjectCache 
{
public:
	DynamicObjectCache(DynamicObjectCacheData<T> * data);
	~DynamicObjectCache();
	
	T * New() { return data->New(); };
	void Delete(T * _item) { return data->Delete(_item); };

private:
	DynamicObjectCacheData<T> * data;
};

	
	
template <class T> 
DynamicObjectCacheData<T>::DynamicObjectCacheData(int _size)
{
	size = _size;
	CreateNewDataBlock();
}

template <class T> 
void DynamicObjectCacheData<T>::CreateNewDataBlock()
{
	T * block = new T[size];
	blocks.push_back(block);
	for (int k = 0; k < size; ++k)
	{
		cache.push_back(&block[k]);
	}
}
	
template <class T> 
void DynamicObjectCacheData<T>::DeallocateMemory()
{
	while(blocks.size() > 0)
	{
		T * object = blocks.front();
		blocks.pop_front();

		delete [] object;
	}
}
	
template <class T> 
DynamicObjectCacheData<T>::~DynamicObjectCacheData()
{
	DeallocateMemory();
}
	
/*template <class T> 
void DynamicObjectCacheData<T>::AddToCache(int size)
{
	for (int k = 0; k < size; ++k)
		data.push_back(New());
}
 */
	
template <class T> 
T * DynamicObjectCacheData<T>::New()
{
	T * object = 0;
	if (cache.empty())
		CreateNewDataBlock();
	
	DVASSERT(cache.size() > 0);
	
	object = cache.front();
	cache.pop_front();
	return object;
}
	
template <class T> 
void DynamicObjectCacheData<T>::Delete(T * object)
{
	cache.push_back(object);
}	
	
};

#endif // __DAVAENGINE_DYNAMIC_OBJECT_CACHE_H__