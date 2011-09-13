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
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTR ACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Ivan "Dizz" Petrochenko
=====================================================================================*/

#ifndef __SPRITETEST_H__
#define __SPRITETEST_H__

#include "DAVAEngine.h"
using namespace DAVA;

#include "TestTemplate.h"

class SpriteTest : public TestTemplate<SpriteTest>
{
public:
	SpriteTest();

	virtual void LoadResources();
	virtual void UnloadResources();

	enum SetDrawType 
	{
		SET_POSITION = 1,
		SET_SCALE = 2,
		SET_ANGLE = 4,
		SET_PIVOT_POINT = 8,
		SET_ALL_DEFAULT_SETTERS = 16,
	};

	void SpriteRGBA888Draw(PerfFuncData * data);
	void GameObjectRGBA888Draw(PerfFuncData * data);
	void SpriteRGBA888DrawStateDraw(PerfFuncData * data);

private:
	Sprite * redSprite;
	Sprite::DrawState redSpriteDrawState;
	GameObject * redGameObject;
	Sprite * greenSprite;
	Sprite * blueSprite;
	Sprite * tranclucentSprite;
	Sprite * zebraSprite;	
};


#endif // __SPRITETEST_H__
