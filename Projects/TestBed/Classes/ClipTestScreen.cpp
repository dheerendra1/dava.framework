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
#include "ClipTestScreen.h"

void ClipTestScreen::LoadResources()
{
	redSprite = Sprite::Create("~res:/Gfx/SpriteBasics/redsprite");
	greenSprite = Sprite::Create("~res:/Gfx/SpriteBasics/greensprite");
	blueSprite = Sprite::Create("~res:/Gfx/SpriteBasics/bluesprite");
	tranclucentSprite = Sprite::Create("~res:/Gfx/SpriteBasics/transclucentsprite");
	zebraSprite = Sprite::Create("~res:/Gfx/SpriteBasics/zebrasprite");
}

void ClipTestScreen::UnloadResources()
{
}

void ClipTestScreen::WillAppear()
{
}

void ClipTestScreen::WillDisappear()
{
	
}

void ClipTestScreen::Input(UIEvent * touch)
{
	if ((touch->phase == UIEvent::PHASE_ENDED) && (touch->tid == UIEvent::BUTTON_2))
	{
		Core::Instance()->ToggleFullscreen();
	}
}

void ClipTestScreen::Draw(const UIGeometricData &geometricData)
{
	RenderManager::Instance()->ClearWithColor(0.0f, 0.3f, 0.0f, 1.0f);

	RenderManager::Instance()->ClipPush();
	RenderManager::Instance()->SetClip(Rect(20, 30, 890, 650));

	RenderManager::Instance()->ClearWithColor(0.0f, 0.0f, 0.5f, 1.0f);

		RenderManager::Instance()->ClipPush();
		RenderManager::Instance()->SetClip(Rect(50, 50, 250, 250));

		RenderManager::Instance()->ClearWithColor(1.0f, 0.0f, 0.0f, 1.0f);

		RenderManager::Instance()->ClipPop();
	RenderManager::Instance()->ClipPop();

	RenderManager::Instance()->SetBlendMode(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);

	redSprite->SetPosition(0.0f, 0.0f);
	redSprite->Draw();
	
	greenSprite->SetPosition(50.0f, 0.0f);
	greenSprite->Draw();
	
	blueSprite->SetPosition(100.0f, 0.0f);
	blueSprite->Draw();
	
	blueSprite->SetPosition(Core::Instance()->GetVirtualScreenWidth() - blueSprite->GetWidth(), 0);
	blueSprite->Draw();

	tranclucentSprite->SetPosition(Core::Instance()->GetVirtualScreenWidth() - blueSprite->GetWidth(), Core::Instance()->GetVirtualScreenHeight() - blueSprite->GetHeight());
	tranclucentSprite->Draw();

	zebraSprite->SetPosition(0, Core::Instance()->GetVirtualScreenHeight() - blueSprite->GetHeight());
	zebraSprite->Draw();

	RenderManager::Instance()->SetColor(1.0f, 1.0f, 1.0f, 0.5f);
	redSprite->SetPosition(0, 50);
	redSprite->Draw();

	greenSprite->SetPosition(50, 50);
	greenSprite->Draw();

	blueSprite->SetPosition(100, 50);
	blueSprite->Draw();
}
