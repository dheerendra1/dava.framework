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
#include "RenderTargetTestScreen.h"
#include "TouchEffectsControl.h"

void RenderTargetTestScreen::LoadResources()
{
	redSprite = Sprite::Create("~res:/Gfx/SpriteBasics/redsprite");
	greenSprite = Sprite::Create("~res:/Gfx/SpriteBasics/greensprite");
	blueSprite = Sprite::Create("~res:/Gfx/SpriteBasics/bluesprite");
	tranclucentSprite = Sprite::Create("~res:/Gfx/SpriteBasics/transclucentsprite");
	zebraSprite = Sprite::Create("~res:/Gfx/SpriteBasics/zebrasprite");
    
	renderTargetRGBA8888 = Sprite::CreateAsRenderTarget(256, 256, Texture::FORMAT_RGBA8888);
	renderTargetRGB565 = Sprite::CreateAsRenderTarget(256, 256, Texture::FORMAT_RGB565);
	renderTargetRGBA4444 = Sprite::CreateAsRenderTarget(256, 256, Texture::FORMAT_RGBA4444);
}

void RenderTargetTestScreen::UnloadResources()
{
	SafeRelease(redSprite);
	SafeRelease(greenSprite);
	SafeRelease(blueSprite);
	SafeRelease(tranclucentSprite);
	SafeRelease(zebraSprite);
}

void RenderTargetTestScreen::WillAppear()
{
	AddControl(new TouchEffectsControl());
}

void RenderTargetTestScreen::WillDisappear()
{
	
}

void RenderTargetTestScreen::Input(UIEvent * touch)
{
	if ((touch->phase == UIEvent::PHASE_ENDED) && (touch->tid == UIEvent::BUTTON_2))
	{
		Core::Instance()->ToggleFullscreen();
	}
}

void RenderTargetTestScreen::Draw(const UIGeometricData &geometricData)
{
	RenderManager::Instance()->ClearWithColor(0.0f, 0.3f, 0.3f, 1.0f);
	RenderManager::Instance()->SetRenderTarget(renderTargetRGBA8888);	
	RenderManager::Instance()->ClearWithColor(0.0f, 0.0f, 0.0f, 1.0f);
	blueSprite->Draw();
	RenderManager::Instance()->RestoreRenderTarget();

	RenderManager::Instance()->SetRenderTarget(renderTargetRGB565);
	RenderManager::Instance()->ClearWithColor(1.0f, 0.0f, 0.0f, 1.0f);
	greenSprite->Draw();
	RenderManager::Instance()->RestoreRenderTarget();
	
	renderTargetRGBA8888->Draw();

	renderTargetRGB565->SetPosition(100, 100);
	renderTargetRGB565->Draw();

	RenderManager::Instance()->SetRenderTarget(renderTargetRGBA4444);
	RenderManager::Instance()->ClearWithColor(0.0f, 0.5f, 0.0f, 1.0f);
	redSprite->Draw();
	tranclucentSprite->SetPosition(15, 15);
	tranclucentSprite->Draw();
	RenderManager::Instance()->RestoreRenderTarget();

	renderTargetRGBA4444->SetPosition(200, 200);
	renderTargetRGBA4444->Draw();

}
