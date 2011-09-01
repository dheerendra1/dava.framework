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
#ifndef __DAVAENGINE_CURSOR_H__
#define __DAVAENGINE_CURSOR_H__

#include "Base/BaseObject.h"
#include "Base/BaseMath.h"
#include "Render/Texture.h"
#include "Render/2D/Sprite.h"

namespace DAVA
{

/**
	\ingroup render
	\brief Hardware cursor for platforms that support mouse
 
	By default there is no cursor set in framework application, if you want to set cursor for your application you need to perform the following code:
	\code
	Cursor * cursor = Cursor::Create("~res:/Cursors/cursor.png", Vector2(0, 0));

	RenderManager::Instance()->SetCursor(cursor);  

	SafeRelease(cursor);
	\endcode
 
	If you want to get current active cursor you should call RenderManager::Instance()->GetCursor();
 */
class Cursor : public BaseObject
{
public:
	/**
		\brief Function to create hardware cursor object
		This function open cursor only if it's in PNG format and it's size should be 32x32 pixels. 

		\param[in] cursorPathname pathname to cursor you want to create
		\param[in] hotSpot position inside cursor image that marks the center of the cursor. The offset is relative to the upper-left corner of the cursor. When the cursor is given a new position, the image is drawn at an offset from this new position determined by subtracting the hot spot coordinates from the position
		\returns pointer to cursor object if it opened successfully. 
	 */
	static Cursor * Create(const String & cursorPathname, const Vector2 & hotSpot);

	/**
	 \brief Show or hide cursor
	 \param[in] show show cursor if true, else hide cursor
	 */
	void Show(bool show);

	/**
	 \brief Get current cursor position 
	 \returns Vector2 containing position
	 */
	Vector2 GetPosition();
	
#if defined(__DAVAENGINE_MACOS__)
	void * GetMacOSXCursor();
#endif 
	
private:
	void HardwareSet();
	
	Cursor();
	~Cursor();
	
#if defined(__DAVAENGINE_MACOS__)
	void * macOSXCursor;
#elif defined(__DAVAENGINE_WIN32__) && defined(__DAVAENGINE_DIRECTX9__)
	Texture * cursorTexture;
	Sprite * cursorSprite;
	Vector2 hotSpot;
public:
	void SoftwareDraw(const Vector2 & pos);

#endif
	
	friend class RenderManager;

	bool show;
};

};

#endif // __DAVAENGINE_CURSOR_H__