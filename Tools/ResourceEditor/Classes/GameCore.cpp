/*
 *  GameCore.cpp
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameCore.h"
#include "AppScreens.h"
#include "ResourcePackerScreen.h"
#include "SceneEditor/SceneEditorScreen.h"

using namespace DAVA;


GameCore::GameCore()
{
}

GameCore::~GameCore()
{
	
}

void GameCore::OnAppStarted()
{
	RenderManager::Instance()->SetFPS(30);
	
	resourcePackerScreen = new ResourcePackerScreen();
    sceneEditorScreen = new SceneEditorScreen();

	UIScreenManager::Instance()->RegisterScreen(SCREEN_RESOURCE_PACKER, resourcePackerScreen);
    UIScreenManager::Instance()->RegisterScreen(SCREEN_SCENE_EDITOR, sceneEditorScreen);
    
	UIScreenManager::Instance()->SetFirst(SCREEN_SCENE_EDITOR);
}

void GameCore::OnAppFinished()
{
	SafeRelease(resourcePackerScreen);
    SafeRelease(sceneEditorScreen);
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
	//ApplicationCore::OnBackground();
}

void GameCore::BeginFrame()
{
	ApplicationCore::BeginFrame();
	RenderManager::Instance()->ClearWithColor(0, 0, 0, 0);
}

void GameCore::Update(float32 timeElapsed)
{	
	ApplicationCore::Update(timeElapsed);
}

void GameCore::Draw()
{
	ApplicationCore::Draw();

}