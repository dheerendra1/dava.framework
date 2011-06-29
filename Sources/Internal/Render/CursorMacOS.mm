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
#include "Base/BaseTypes.h"
#include "Render/Cursor.h"
#include "FileSystem/FileSystem.h"

#if defined(__DAVAENGINE_MACOS__) 

#include <Cocoa/Cocoa.h>

namespace DAVA
{
Cursor * Cursor::Create(const String & cursorPathname, const Vector2 & hotSpot)
{
	const String & realPath = FileSystem::Instance()->SystemPathForFrameworkPath(cursorPathname);
	NSImage * image = [[NSImage alloc] initByReferencingFile:[NSString stringWithFormat:@"%s", realPath.c_str()]];
	if (!image)
	{
		Logger::Error("Can't open cursor image");
		return 0;
	}
	
	/*if ((image.size.width != 32) && (image.size.height != 32))
	{
		Logger::Error("You are trying to create cursor image with size different from 32x32 pixels");
		return 0;
	}*/
	
	NSCursor * macOSXCursor = [[NSCursor alloc] initWithImage:image hotSpot:NSMakePoint(hotSpot.x, hotSpot.y)];
	[macOSXCursor setOnMouseEntered: YES];
	
	[image release];
	image = 0;
	
	if (macOSXCursor)
	{
		Cursor * cursor = new Cursor();
		cursor->macOSXCursor = macOSXCursor;
		return cursor;
	}
	return 0;
}

Cursor::Cursor()
{
}

Cursor::~Cursor()
{
	NSCursor * macOSXCursorX = (NSCursor *) this->macOSXCursor;
	[macOSXCursorX release];
	this->macOSXCursor = 0;
}
	
void * Cursor::GetMacOSXCursor()
{
	return macOSXCursor;
}
	
void Cursor::HardwareSet()
{
	// Do nothing here in MacOS version. Everything is in OpenGLView 
}
	
/*void Cursor::MacOSX_Set()
{
	NSCursor * macOSXCursorX = (NSCursor *)this->macOSXCursor;
	[macOSXCursorX set];
}*/
	
};


#endif // __DAVAENGINE_MACOS__