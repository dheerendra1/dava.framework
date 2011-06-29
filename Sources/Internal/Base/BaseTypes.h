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
#ifndef __DAVAENGINE_TYPES_H__
#define __DAVAENGINE_TYPES_H__

// debug definition
#if defined(DAVA_DEBUG)
#define __DAVAENGINE_DEBUG__
#endif

// Platform detection:

#if defined(__GNUC__) && ( defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__MACOS_CLASSIC__) )
//#if !defined(_WIN32) // fix for mac os platforms
#include <TargetConditionals.h>
#endif


#if defined(TARGET_OS_IPHONE)
#if TARGET_OS_IPHONE
	#if !defined(__DAVAENGINE_IPHONE__) // for old projects we check if users defined it
		#define __DAVAENGINE_IPHONE__
	#endif
#endif
#endif


#ifndef __DAVAENGINE_IPHONE__
#if defined(_WIN32)
#define __DAVAENGINE_WIN32__
//#elif defined(__APPLE__) || defined(MACOSX)
#elif defined(__GNUC__) && ( defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__MACOS_CLASSIC__) )
#define __DAVAENGINE_MACOS__
#endif
#endif


// add some other platform detection here...


/////////
// Default headers per platform:


#if defined(__DAVAENGINE_WIN32__)
#define __DAVASOUND_AL__
#define WIN32_LEAN_AND_MEAN
//#include <windef.h>
#include <windows.h>
#include <windowsx.h>
#undef DrawState
#undef GetCommandLine

#elif defined(__DAVAENGINE_IPHONE__) || defined(__DAVAENGINE_MACOS__) // Mac & iPhone
#define __DAVASOUND_AL__

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>

#else
// some other platform...

#endif 


//#define _HAS_ITERATOR_DEBUGGING 0
//#define _SECURE_SCL 0

// MSVS: conversion from type1 to type2, possible loss of data

#if defined(__DAVAENGINE_WIN32__)
#pragma warning( push )
#pragma warning( disable : 4244)
#endif 

#include <string>
#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <stack>

#if defined(__DAVAENGINE_WIN32__)
#pragma warning( pop )
#endif 

//#if TARGET_OS_IPHONE_SIMULATOR //) || defined(TARGET_IPHONE)
//#if defined(__APPLE__) || defined(MACOSX)
//#define __IPHONE__
//#endif 
//#define __DAVAENGINE_IPHONE__



namespace DAVA
{

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

typedef signed char		int8;
typedef signed short	int16;
typedef signed int		int32;

#if defined(_WIN32)
	typedef unsigned __int64 uint64;
	typedef signed __int64	int64;
#else
	typedef unsigned long long uint64;
	typedef signed long long int64;
#endif 
	
#ifndef TRUE
#define TRUE	1
#endif
	
#ifndef FALSE
#define	FALSE	0
#endif

typedef char		char8;
typedef wchar_t		char16;

typedef float			float32;
typedef double			float64;

typedef std::string		String;
typedef std::wstring	WideString;
	

//template <typename _Ty, typename _Ax = std::allocator(_Ty)> 
//class List : public std::list<_Ty, _Ax>  {};


//#define List std::list
//#define Vector std::vector
template < class E > class List : public std::list< E > {};
template < class E > class Vector : public std::vector< E > {};
template < class E > class Set : public std::set< E > {};
template < class E > class Deque : public std::deque< E > {};

template<	class _Kty,
			class _Ty,
			class _Pr = std::less<_Kty>,
			class _Alloc = std::allocator<std::pair<const _Kty, _Ty> > >

class Map : public std::map<_Kty, _Ty, _Pr, _Alloc> {};

template < class T, class Container = std::deque<T> > class Stack : public std::stack< T, Container > {};

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

template <class T>
inline T Min(T a, T b)
{
	return (a < b) ? (a) : (b);
}

template <class T>
inline T Max(T a, T b)
{
	return (a > b) ? (a) : (b);
}
	
template <class T>
inline T Abs(T a)
{
	return (a >= 0) ? (a) : (-a);
}

template <class T>
inline T Clamp(T val, T a, T b)
{
	return Min(b, Max(val, a));
}
	


#define Memcpy memcpy
#define Memset memset
#define Alloc malloc
#define Free free
#define Realloc realloc

template <class TYPE>
void SafeDelete(TYPE * &d)
{
	if (d)
	{
		delete d;
		d = 0;
	}
}

template <class TYPE>
void SafeDeleteArray(TYPE * & d)
{
	if (d)
	{
		delete [] d;
		d = 0;
	}
}

#ifndef SAFE_DELETE // for compatibility with FCollada
#define SAFE_DELETE(x) if (x) { delete x; x = 0; };
#endif 
	
#ifndef SAFE_DELETE_ARRAY // for compatibility with FCollada
#define SAFE_DELETE_ARRAY(x) if (x) { delete [] x; x = 0; };
#endif
	
#ifndef OBJC_SAFE_RELEASE
#define OBJC_SAFE_RELEASE(x) [x release];x = nil;
#endif 
	
	/**
	 \enum Graphical object aligment.
	 */
enum eAlign 
{
	ALIGN_LEFT		= 0x01,	//!<Align graphical object by the left side.
	ALIGN_HCENTER	= 0x02,	//!<Align graphical object by the horizontal center.
	ALIGN_RIGHT		= 0x04,	//!<Align graphical object by the right side.
	ALIGN_TOP		= 0x08,	//!<Align graphical object by the top side.
	ALIGN_VCENTER	= 0x10,	//!<Align graphical object by the vertical center.
	ALIGN_BOTTOM	= 0x20,	//!<Align graphical object by the bottom side.
	ALIGN_HJUSTIFY	= 0x40	//!<Used only for the fonts. Stretch font string over all horizontal size of the area.
};
};



#endif

