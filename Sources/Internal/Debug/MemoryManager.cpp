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
#include "Debug/MemoryManager.h"
#include "Debug/List.h"
#include "FileSystem/Logger.h"



#ifdef ENABLE_MEMORY_MANAGER

#ifdef new
#undef new
#endif

namespace DAVA 
{
#define CHECK_CODE 0xCEECC0DE
	
struct MemoryBlock
{
	char			* filename;
	int				line;
	int				size;
	bool			isPlaced;  // C++PL 10.4.11
	void			* ptr;
};

	


	
class MemoryManagerImpl : public MemoryManager
{
public:
	MemoryManagerImpl();
	virtual ~MemoryManagerImpl();
	
	virtual void	*New(size_t size, const char * _file, int _line);
	virtual void	*New(size_t size, void *pLoc, const char * _file, int _line);
	virtual void	Delete(void * pointer);
	
	virtual void	CheckMemoryLeaks();
	virtual void	FinalLog();
	void	CheckMemblockOverrun(MemoryBlock & memBlock);

	static MemoryManagerImpl * Instance() 
	{
		//static MemoryManagerImpl instance;
		//return &instance;
		if (instance_new == 0)
		{
			void * addr = malloc(sizeof(MemoryManagerImpl));
			instance_new = new(addr) MemoryManagerImpl(); //(sizeof(MemoryManagerImpl), addr);
		}
		return instance_new;					 
	} 
	MallocList<MemoryBlock> blockList;
	
	// Statistics
	int		peakMemoryUsage;
	int		maximumBlockSize;
	int		currentAllocatedMemory;
	
	static int useClass;
	static MemoryManagerImpl * instance_new;
};
	
	int MemoryManagerImpl::useClass = 0;
	MemoryManagerImpl * MemoryManagerImpl::instance_new = 0;


}

/*void*	operator new(size_t _size, void* pLoc, const char *_file, int _line)
{
	return MemManager->New(_size, pLoc, _file, _line);
}

void*	operator new( size_t _size, const char *_file, int _line)
{
	return MemManager->New(_size, _file, _line);
}

void	operator delete(void * ptr)
{
	MemManager->Delete(ptr);
}

void*	operator new[]( size_t _size, const char *_file, int _line)
{
	return MemManager->New(_size, _file, _line);
}

void	operator delete[](void * ptr)
{
	MemManager->Delete(ptr);
}
*/

#ifdef new
#undef new
#endif 

void*	operator new( size_t _size, const char *_file, int _line)
{
	return DAVA::MemoryManagerImpl::Instance()->New(_size, _file, _line);
}

void*	operator new[]( size_t _size, const char *_file, int _line)
{
	return DAVA::MemoryManagerImpl::Instance()->New(_size, _file, _line);
}

void * operator new(size_t _size)
{
	return DAVA::MemoryManagerImpl::Instance()->New(_size, 0, 0);
}
void   operator delete(void * _ptr)
{
	DAVA::MemoryManagerImpl::Instance()->Delete(_ptr);
}
void * operator new[](size_t _size)
{
	return DAVA::MemoryManagerImpl::Instance()->New(_size, 0, 0);
}
void   operator delete[](void * _ptr)
{
	DAVA::MemoryManagerImpl::Instance()->Delete(_ptr);
}

void*	operator new(size_t _size, void* pLoc, const char *_file, int _line)
{
	return DAVA::MemoryManagerImpl::Instance()->New(_size, pLoc, _file, _line);
}

namespace DAVA 
{
	


MemoryManagerImpl::MemoryManagerImpl()
{
	//memoryLog.open("memory.log");
	//memoryLog << "          M E M O R Y   M A N A G E R   L O G " << std::endl;
	//memoryLog << "----------------------------------------------------------------------" << std::endl;
	// clean statistics
	currentAllocatedMemory = 0;
	peakMemoryUsage = 0;
	maximumBlockSize = 0;
}

MemoryManagerImpl::~MemoryManagerImpl()
{
}

void	*MemoryManagerImpl::New(size_t size, const char * _file, int _line)
{
	void * returnPtr = 0;
#ifndef CHECK_MEMORY_OVERRUNS
	void* allocPtr = malloc (size);
	returnPtr = allocPtr;
#else
	if ((size & 3) != 0)
	{
		size += 4 - (size & 3);
	}
	
	void * allocPtr = malloc (size + CHECK_MEMORY_OVERRUNS * 2 * 4);
	uint32 * fill = (uint32*)allocPtr;
	uint32 * fillEnd = (uint32*)allocPtr + CHECK_MEMORY_OVERRUNS * 2 + (size / 4) - 1;
	for (int k = 0; k < CHECK_MEMORY_OVERRUNS; ++k)
	{
		*fill++ = CHECK_CODE; 
		*fillEnd-- = CHECK_CODE;
	}
	returnPtr = (uint32*)allocPtr + CHECK_MEMORY_OVERRUNS;
#endif	

	if (allocPtr)
	{
		MemoryBlock memBlock;
		
		memBlock.filename = 0;
		if (_file)
		{
			int len = strlen(_file);
			memBlock.filename = (char *)malloc(len + 1);
			strncpy(memBlock.filename,  _file, len + 1);
		}
		memBlock.line = _line;
		memBlock.size = size;
		memBlock.isPlaced = false;
		memBlock.ptr = allocPtr;
		currentAllocatedMemory += size;
		//memoryLog << " * block : " << size << std::endl;


		if (currentAllocatedMemory > peakMemoryUsage)
		{
			peakMemoryUsage = currentAllocatedMemory;
		}
		if (size > maximumBlockSize)
			maximumBlockSize = size;
		blockList.PushBack(memBlock);
	}
	
	return returnPtr;
}

void	*MemoryManagerImpl::New(size_t size, void *pLoc, const char * _file, int _line)
{
	if (pLoc)
	{
		MemoryBlock memBlock;
		memBlock.filename = 0;
		if (_file)
		{
			int len = strlen(_file);
			memBlock.filename = (char *)malloc(len + 1);
			strncpy(memBlock.filename,  _file, len + 1);
		}
		memBlock.line = _line;
		memBlock.size = size;
		memBlock.isPlaced = true;
		memBlock.ptr = 0;
		//currentAllocatedMemory += size;
	
		if (currentAllocatedMemory > peakMemoryUsage)
		{
			peakMemoryUsage = currentAllocatedMemory;
		}
		if (size > maximumBlockSize)
			maximumBlockSize = size;

		blockList.PushBack(memBlock);
	}

	return pLoc;
}
	
void	MemoryManagerImpl::CheckMemblockOverrun(MemoryBlock & memBlock)
{
#ifdef CHECK_MEMORY_OVERRUNS
	uint32 * fill = (uint32*)memBlock.ptr;
	uint32 * fillEnd = (uint32*)memBlock.ptr + CHECK_MEMORY_OVERRUNS * 2 + (memBlock.size / 4) - 1;
	for (int k = 0; k < CHECK_MEMORY_OVERRUNS; ++k)
	{
		if (*fill != CHECK_CODE)
		{
			Logger::Error("* Find head overrun for block allocated: %s, line:%d", memBlock.filename, memBlock.line);
			*fill++; 
		}
		if (*fillEnd != CHECK_CODE)
		{
			Logger::Error("* Find tail overrun for block allocated: %s, line:%d", memBlock.filename, memBlock.line);
			*fillEnd--;
		}
	}
	
#endif 
}
void	MemoryManagerImpl::Delete(void * ptr)
{
	if (ptr)
	{
#ifdef CHECK_MEMORY_OVERRUNS		
		ptr = ((char*)ptr - CHECK_MEMORY_OVERRUNS * 4);
#endif 
		for (MallocList<MemoryBlock>::Iterator it = blockList.Begin(); it != blockList.End(); ++it)
			if (it->ptr == ptr)
			{
				MemoryBlock & memBlock = *it;
				if(!memBlock.isPlaced)
				{
					CheckMemblockOverrun(memBlock);
					free(memBlock.ptr); //!!!
					currentAllocatedMemory -= memBlock.size;
				}
				if (memBlock.filename)
					free(memBlock.filename);
				
				blockList.Erase(it);
				break;
			}
	}
}


void	MemoryManagerImpl::FinalLog()
{
	Logger::Debug("    M E M O R Y     M A N A G E R     R E P O R T   ");
	Logger::Debug("----------------------------------------------------");
	// 
	Logger::Debug("* Currently Allocated Memory Size : %d", currentAllocatedMemory);
	Logger::Debug("* Memory Leaks Count: %d", blockList.Size());
	Logger::Debug("* Peak Memory Usage : %d", peakMemoryUsage);
	Logger::Debug("* Max Allocated Block Size : %d", maximumBlockSize);
	CheckMemoryLeaks();
}

void	MemoryManagerImpl::CheckMemoryLeaks()
{
	Logger::Debug(" C U R R E N T L Y   A L L O C A T E D  M E M O R Y ");
	Logger::Debug("----------------------------------------------------");

	for (MallocList<MemoryBlock>::Iterator it = blockList.Begin(); it != blockList.End(); ++it)
	{
		MemoryBlock & block = *it;
		CheckMemblockOverrun(block);
		if (block.filename)
		{
			Logger::Debug("****** 	Memory Leak Found	******");
			Logger::Debug("* Source File : %s", block.filename);
			Logger::Debug("* Source Line : %d", block.line);
			Logger::Debug("* Allocation Size : %d bytes", block.size);
			//Logger::Debug("* Was placed : " << (block.isPlaced ? " true" : " false")<< std::endl;
			
			Logger::Debug("************************************");
		}
	}
}

};

#endif // MEMORY_MANAGER

