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
#ifndef __DAVAENGINE_LIST_H__
#define __DAVAENGINE_LIST_H__

#include "Base/BaseTypes.h"
#include "Debug/ListAllocator.h"

namespace DAVA 
{


//! \brief Class MallocList for INTERNAL MEMORY MANAGER usage
//! this class uses Malloc to allocate nodes
template <class ITEM>
class MallocList
{
public:
	struct Node
	{
		ITEM i;
		Node * prevI;
		Node * nextI;
	};

public:

	//! \brief Class List Iterator
	class Iterator
	{
		friend class MallocList;

	public:

		// ***************************************************
		//! \brief    	operator++ - postincrement the iterator
		//! 
		//! \return   	Iterator &
		// ***************************************************
		Iterator & operator++()
		{
			cNode = cNode->nextI;
			return *this;
		}

		// ***************************************************
		//! \brief    	operator++ - preincrement the iterator
		//! 
		//! \return   	MallocList::Iterator
		// ***************************************************
		Iterator operator++(int)
		{
			Iterator i = *this;
			++(*this);
			return i;
		}

		// ***************************************************
		//! \brief    	operator-- - postdecrement the iterator
		//! 
		//! \return   	Iterator &
		// ***************************************************
		Iterator & operator--()
		{
			cNode = cNode->prevI;
			return *this;
		}
		
		// ***************************************************
		//! \brief    	operator-- - predecrement the iterator
		//! 
		//! \return   	MallocList::Iterator
		// ***************************************************
		Iterator operator--(int)
		{
			Iterator i = *this;
			--(*this);
			return i;
		}
		
		// ***************************************************
		//! \brief    	operator* - get item pointed by the iterator
		//! 
		//! \return   	ITEM &
		// ***************************************************
		ITEM & operator*()
		{
			return this->cNode->i;
		}
		
		// ***************************************************
		//! \brief    	operator-> - get the item pointer pointed by the iterator 
		//! 
		//! \return   	ITEM *
		// ***************************************************
		ITEM * operator->()
		{
			return &this->cNode->i;
		}
		
		// ***************************************************
		//! \brief    	operator= - assigns iterator value
		//! 
		//! \param      i
		//! \return   	Iterator &
		// ***************************************************
		Iterator & operator=(const Iterator & i)
		{
			this->cNode = i.cNode;
			return *this;
		}
		
		// ***************************************************
		//! \brief    	operator!= - test for iterator inequality 
		//! 
		//! \param      i
		//! \return   	bool
		// ***************************************************
		bool operator != (const Iterator & i) const
		{
			return (this->cNode != i.cNode);
		}
		
		// ***************************************************
		//! \brief    	operator== - test for iterator equality 
		//! 
		//! \param      i
		//! \return   	bool
		// ***************************************************
		bool operator == (const Iterator & i) const
		{
			return (this->cNode == i.cNode);
		}

	private:

		typename MallocList<ITEM>::Node * cNode;
	};
	
	// ***************************************************
	//! \brief    	MallocList - Default constructor
	// ***************************************************
	MallocList()
	{
		allocator = Allocator<Node>();
		size = 0;
		head = allocator.Alloc();
		head->prevI = head;
		head->nextI = head;
	}

	
	// ***************************************************
	//! \brief    	MallocList
	//! 
	//! \param      _reserve - amount of items reserved in the list allocator 	
	// ***************************************************
	MallocList(int _reserve)
	{
		allocator = Allocator<Node>();
		size = 0;
		head = allocator->Alloc();
		head->prevI = head;
		head->nextI = head;
	}

	MallocList(const MallocList & _list)
	{
		// Not using copying constructor
	}

	// ***************************************************
	//! \brief    	~MallocList - destructor
	// ***************************************************
	~MallocList()
	{
		Clear();
		allocator.Dealloc(head);
	}

	// ***************************************************
	//! \brief    	operator= - Copies all the elements of the source list to itself
	//! 
	//! \param      l - source list
	//! \return   	MallocList &
	// ***************************************************
	MallocList & operator=(MallocList & l);

	// ***************************************************
	//! \brief    	Empty - test for list emptiness
	//! 
	//! \return   	bool
	// ***************************************************
	bool Empty();

	// ***************************************************
	//! \brief    	Resize - Resizes the list 
	//! 
	//! \param      size - New size of the list
	//! \return   	void
	// ***************************************************
	void Resize(int32 size);

	// ***************************************************
	//! \brief    	Size - returns the amount of list elements
	//! 
	//! \return   	int32
	// ***************************************************
	int32 Size();

	// ***************************************************
	//! \brief    	Begin - Returns the iterator to the first element of the list
	//! 
	//! \return   	Iterator
	// ***************************************************
	Iterator Begin();

	// ***************************************************
	//! \brief    	End - Returns the iterator to the last element of the list
	//! 
	//! \return   	Iterator
	// ***************************************************
	Iterator End();

	// ***************************************************
	//! \brief    	Pos - returns the iterator to the list element by index
	//! 
	//! \param      _index - element index
	//! \return   	Iterator
	// ***************************************************
	Iterator Pos(int32 _index);

	// ***************************************************
	//! \brief    	PosItem - the function returns the iterator to the set element
	//! 
	//! \param      item - sets the element
	//! \return   	Iterator
	// ***************************************************
	Iterator PosItem(const ITEM & item);
	
	// ***************************************************
	//! \brief    	PushFront - Adds a new element to the beginning of the list
	//! 
	//! \param      i - the element
	//! \return   	void
	// ***************************************************
	void PushFront(const ITEM & i);

	// ***************************************************
	//! \brief    	PopFront - Removes the element from the beginning of the list
	//! 
	//! \return   	void
	// ***************************************************
	void PopFront();
	
	// ***************************************************
	//! \brief    	PushBack - Adds a new element to the end of the list
	//! 
	//! \param      i - the element
	//! \return   	void
	// ***************************************************
	void PushBack(const ITEM & i);
	
	// ***************************************************
	//! \brief    	PopBack - Removes the element from the end of the list
	//! 
	//! \return   	void
	// ***************************************************
	void PopBack();
	
	// ***************************************************
	//! \brief    	Insert - Inserts the element before the element pointed by the iterator
	//! 
	//! \param      i - the iterator
	//! \param      it - the element
	//! \return   	Iterator
	// ***************************************************
	Iterator Insert(Iterator & i, const ITEM & it);
	
	// ***************************************************
	//! \brief    	Erase - Removes the element pointed by the iterator
	//! 
	//! \param      i - the iterator
	//! \return   	Iterator
	// ***************************************************
	Iterator Erase(Iterator & i);
	
	// ***************************************************
	//! \brief    	Clear - Clears all the list
	//! 
	//! \return   	void
	// ***************************************************
	void Clear();

private:

	Node * head;

	Allocator<Node> allocator;

	int32 size;
};

// Class implementation

template <class T>
MallocList<T> & MallocList<T>::operator=(MallocList<T> & l)
{
	this->Clear();
	Iterator firstSrc = l.Begin();
	Iterator lastSrc = l.End();
	Iterator lastDst = End();

	for (; firstSrc != lastSrc; ++firstSrc)
	{
		Insert(lastDst, *firstSrc);
	}
	return *this;
}

template <class T>
bool MallocList<T>::Empty()
{
	return (head == head->nextI);
}

template <class T>
void MallocList<T>::Resize(int32 size)
{
	if (this->size < size)
		while (this->size < size)
			PushBack(T());
	else
		while (this->size > size)
			PopBack();
}

template <class T>
int32 MallocList<T>::Size()
{
	return size;
}

template <class T>
typename MallocList<T>::Iterator MallocList<T>::Begin()
{
	Iterator i;
	i.cNode = head->nextI;
	return i;
}

template <class T>
typename MallocList<T>::Iterator MallocList<T>::End()
{
	Iterator i;
	i.cNode = head;
	return i;
}

template <class T>
typename MallocList<T>::Iterator MallocList<T>::Pos(int32 _index)
{
	Iterator it = Begin();

	for (int32 i = 0; i < _index; ++i)
		it++;

	return it;
}

template <class T>
typename MallocList<T>::Iterator MallocList<T>::PosItem(const T & _item)
{
	Iterator it = Begin();
	for (; it != End(); ++it)
	{
		if (*it == _item)
			return it;
	}

	return it;
}

template <class T>
void MallocList<T>::PushFront(const T & i)
{
	typename MallocList<T>::Iterator begin = Begin();
	Insert(begin, i);
}

template <class T>
void MallocList<T>::PopFront()
{
	typename MallocList<T>::Iterator begin = Begin();
	Erase(begin);
}

template <class T>
void MallocList<T>::PushBack(const T & i)
{
	typename MallocList<T>::Iterator end = End();
	Insert(end, i);
}

template <class T>
void MallocList<T>::PopBack()
{
	typename MallocList<T>::Iterator end = End();
	end--;
	Erase(end);
}

template <class T>
typename MallocList<T>::Iterator MallocList<T>::Insert(Iterator & i, const T & it)
{
	typename MallocList<T>::Iterator newIt;
	Node * pos = i.cNode;
	Node * newpos = allocator.Alloc();
	newpos->i = it;
	newpos->nextI = pos;
	newpos->prevI = pos->prevI;
	pos->prevI->nextI = newpos;
	pos->prevI = newpos;
	size++;
	newIt.cNode = newpos;
	return newIt;
}

template <class T>
typename MallocList<T>::Iterator MallocList<T>::Erase(Iterator & i)
{
	typename MallocList<T>::Iterator newIt;
	Node * pos = i.cNode;
	if (pos == head)
		return i;

	pos->prevI->nextI = pos->nextI;
	pos->nextI->prevI = pos->prevI;

	newIt.cNode = pos->nextI;

	allocator.Dealloc(pos);
	size--;

	return newIt;
}

template <class T>
void MallocList<T>::Clear()
{
	while (Size())
	{
		typename MallocList<T>::Iterator begin = Begin();
		Erase(begin);
	}
}
	
};


#endif // __DAVAENGINE_LIST_H__