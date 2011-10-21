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
#ifndef __LOGENGINE_ASSERT_H__
#define __LOGENGINE_ASSERT_H__

#include "Base/BaseTypes.h"
#include "FileSystem/Logger.h"


namespace DAVA
{
	
/**
	\page tutorial_debug Debugging
	Here you'll learn how to debug your project and find bugs faster.

	\section asserts Assertion macros 
	For debugging purposes and for easy search of bugs in runtime you can use assert macros. 
	There are 2 types of macros defined: DVASSERT and DVVERIFY.

	DVASSERT macro designed for situations where you want to check something but in release you want to remove this check at all. 

	For example, you have a function SetFrame and frame can't be negative, but you want to check it only in debug, and stop execution if such situation happened. 
	You can write
	\code
	void SetFrame(int32 frame)
	{
		DVASSERT(frame >= 0);		// this code will be removed in release configuration.

		// Function code
	}
	\endcode

	In case if you execute some function inside your assertion and want to leave the calls but remove checks you should 
	use DVVERIFY macro. 

	\code
	void SomeFunction(BaseObject * object)
	{
		int32 propertyInt;
		DVVERIFY(GetObjectProperty(object, "propertyInt", &propertyInt));		// this code will not be removed in release configuration.
	}
	\endcode
*/


// Alexandresky style compile time assertion. 
template <bool> struct CompileTimeError;
template <> struct CompileTimeError<true> {};
#define COMPILER_ASSERT(expr)  (CompileTimeError<(expr)!=0>());
	
// Runtime asserts	


#if defined(__DAVAENGINE_WIN32__)
#define DebugBreak() { __debugbreak(); }
#elif defined(__DAVAENGINE_IPHONE__) || defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_ANDROID__) // Mac & iPhone & Android
#include <signal.h>
#define DebugBreak() { kill( getpid(), SIGINT ) ; }
#else //PLATFORMS
	//other platforms
#endif //PLATFORMS
	
#if defined(__DAVAENGINE_DEBUG__)

#define DVASSERT(expr)\
	if (!(expr))\
	{\
		Logger::Instance()->Warning("*** Warning : DV_ASSERT Expression(%s),\n                         File(%s), Line(%d)\n", #expr, __FILE__, __LINE__);\
		DebugBreak()\
	}\

#define DVASSERT_MSG(expr, msg)\
	if (!(expr))\
	{\
		Logger::Instance()->Warning("*** Warning : DV_ASSERT Expression(%s) msg(%s),\n                         File(%s), Line(%d)\n", #expr, msg, __FILE__, __LINE__);\
		DebugBreak()\
	}\

#define DVVERIFY(expr) DVASSERT(expr)

#else // no assert functions in release builds

#define DVASSERT(expr)
#define DVASSERT_MSG(expr, msg)
	
#define DVVERIFY(expr) (expr)

#endif
	


}; // end of namespace Log

#endif

