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
#include "GameCore.h"
#include "AppScreens.h"
#include "TestScreen.h"
#include "AnimationTestScreen.h"
#include "SpriteBasicsScreen.h"
#include "ClipTestScreen.h"
#include "RenderTargetTestScreen.h"
#include "ParticleTestScreen.h"
#include "SpritePerfScreen.h"
#include "GameObjectTestScreen.h"
#include "FontUsageScreen.h"
#include "Box2DGameObjectTestScreen.h"
#include "StaticTextScreen.h"
#include "CollisionTestScreen.h"
#include "ImageUsageScreen.h"
#include "LandscapeTestScreen.h"
#include "TextinputTestScreen.h"

using namespace DAVA;

GameCore::GameCore()
{
	
}

GameCore::~GameCore()
{
	
}

void GameCore::OnAppStarted()
{
//	RenderManager::Instance()->SetFPS(60);

	Logger::Debug("long %d", sizeof(long));
	Logger::Debug("long long %d", sizeof(long long));



// 	testScreen = new TestScreen();
 	anim3dScreen = new AnimationTestScreen();
 	spriteBasicsScreen = new SpriteBasicsScreen();
// 	clipTestScreen = new ClipTestScreen();
 	renderTargetTestScreen = new RenderTargetTestScreen();
	particleTestScreen = new ParticleTestScreen();
// 	spritePerfScreen = new SpritePerfScreen();
// 	gameObjectTestScreen = new GameObjectTestScreen();
// 	fontUsageScreen = new FontUsageScreen();
// 	box2dGameObjectTestScreen = new Box2DGameObjectTestScreen();
 	stScreen = new StaticTextScreen();
// 	collisionTestScreen = new CollisionTestScreen();
	imageUsageScreen = new ImageUsageScreen();
#if defined(__DAVAENGINE_WIN32__) //Dizz: cause LandscapeTestScreen doesn't work on Windows, similar #ifdef in LandscapeTestScreen.cpp
	landscapeTestScreen = 0;
#else
	landscapeTestScreen = new LandscapeTestScreen();
#endif
	
	textinputTestScreen1 = new TextinputTestScreen(L"textinputTestScreen 1");
	textinputTestScreen2 = new TextinputTestScreen(L"textinputTestScreen 2");
    
//	UIScreenManager::Instance()->RegisterScreen(SCREEN_TEST, testScreen);
//	UIScreenManager::Instance()->RegisterScreen(SCREEN_ANIM_3D, anim3dScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_SPRITE_BASICS, spriteBasicsScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_CLIP_TEST, clipTestScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_RENDERTARGET_TEST, renderTargetTestScreen);
	UIScreenManager::Instance()->RegisterScreen(SCREEN_PARTICLE_TEST, particleTestScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_SPRITE_PERF_TEST, spritePerfScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_GAME_OBJECTS_TEST, gameObjectTestScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_FONT_USAGE, fontUsageScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_BOX2D_GAME_OBJECTS_TEST, box2dGameObjectTestScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_STATIC_TEXT, stScreen);
// 	UIScreenManager::Instance()->RegisterScreen(SCREEN_COLLISION_TEST, collisionTestScreen); 
	UIScreenManager::Instance()->RegisterScreen(SCREEN_IMAGE_USAGE, imageUsageScreen);
    UIScreenManager::Instance()->RegisterScreen(SCREEN_LANDSCAPE_TEST, landscapeTestScreen);    
	UIScreenManager::Instance()->RegisterScreen(SCREEN_TEXTINPUT_TEST_1, textinputTestScreen1);   
	UIScreenManager::Instance()->RegisterScreen(SCREEN_TEXTINPUT_TEST_2, textinputTestScreen2);

//	UIScreenManager::Instance()->SetFirst(SCREEN_TEXTINPUT_TEST_1);
    
	UIScreenManager::Instance()->SetFirst(SCREEN_LANDSCAPE_TEST);
	cursor = 0;
}

void GameCore::OnAppFinished()
{
	SafeRelease(cursor);

	SafeRelease(textinputTestScreen1);
	SafeRelease(textinputTestScreen2);
    SafeRelease(landscapeTestScreen);
//	SafeRelease(collisionTestScreen);
// 	SafeRelease(stScreen);
// 	SafeRelease(box2dGameObjectTestScreen);
// 	SafeRelease(fontUsageScreen);
// 	SafeRelease(testScreen);
 	SafeRelease(anim3dScreen);
 	SafeRelease(spriteBasicsScreen);
// 	SafeRelease(clipTestScreen);
 	SafeRelease(renderTargetTestScreen);
	SafeRelease(particleTestScreen);
    SafeRelease(stScreen);
// 	SafeRelease(spritePerfScreen);
// 	SafeRelease(gameObjectTestScreen);
	SafeRelease(imageUsageScreen);
}

void GameCore::OnSuspend()
{
    ApplicationCore::OnSuspend();
	
}

void GameCore::OnResume()
{
    ApplicationCore::OnResume();
}

void GameCore::OnBackground()
{
	
}

void GameCore::BeginFrame()
{
	ApplicationCore::BeginFrame();
	RenderManager::Instance()->ClearWithColor(0, 0, 0, 0);
}

void GameCore::Update(float32 timeElapsed)
{	
//	if (!cursor)
//	{
//		cursor = Cursor::Create("~res:/Cursor/cursor1.png", Vector2(6, 0));
//		RenderManager::Instance()->SetCursor(cursor);
//	}
	ApplicationCore::Update(timeElapsed);
}

void GameCore::Draw()
{
	ApplicationCore::Draw();
}