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
#include "SpritePerfScreen.h"
#include "TouchEffectsControl.h"

void SpritePerfScreen::LoadResources()
{
	redSprite = Sprite::Create("~res:/Gfx/SpriteBasics/redsprite");
	greenSprite = Sprite::Create("~res:/Gfx/SpriteBasics/greensprite");
	blueSprite = Sprite::Create("~res:/Gfx/SpriteBasics/bluesprite");
	tranclucentSprite = Sprite::Create("~res:/Gfx/SpriteBasics/transclucentsprite");
	zebraSprite = Sprite::Create("~res:/Gfx/SpriteBasics/zebrasprite");
	
	redGameObject = GameObject::Create("~res:/Gfx/SpriteBasics/redsprite");
	RestartTest();
}

void SpritePerfScreen::RestartTest()
{	
	perfFuncs.clear();
	
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position)", SET_POSITION);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position | all default setters)", SET_POSITION | SET_ALL_DEFAULT_SETTERS);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position | pivot)", SET_POSITION | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position | angle)", SET_POSITION | SET_ANGLE);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position | scale)", SET_POSITION | SET_SCALE);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position | angle | pivot)", SET_POSITION | SET_ANGLE | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position | scale | pivot)", SET_POSITION | SET_SCALE | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position | angle | scale)", SET_POSITION | SET_SCALE | SET_ANGLE);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888Draw, "SpriteRGBA888Draw(position | angle | scale | pivot)", SET_POSITION | SET_SCALE | SET_ANGLE | SET_PIVOT_POINT);

	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888DrawStateDraw, "SpriteRGBA888DrawStateDraw(position)", SET_POSITION);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888DrawStateDraw, "SpriteRGBA888DrawStateDraw(position | pivot)", SET_POSITION | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888DrawStateDraw, "SpriteRGBA888DrawStateDraw(position | angle)", SET_POSITION | SET_ANGLE);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888DrawStateDraw, "SpriteRGBA888DrawStateDraw(position | scale)", SET_POSITION | SET_SCALE);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888DrawStateDraw, "SpriteRGBA888DrawStateDraw(position | angle | pivot)", SET_POSITION | SET_ANGLE | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888DrawStateDraw, "SpriteRGBA888DrawStateDraw(position | scale | pivot)", SET_POSITION | SET_SCALE | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888DrawStateDraw, "SpriteRGBA888DrawStateDraw(position | angle | scale)", SET_POSITION | SET_SCALE | SET_ANGLE);
	RegisterPerfFunc(&SpritePerfScreen::SpriteRGBA888DrawStateDraw, "SpriteRGBA888DrawStateDraw(position | angle | scale | pivot)", SET_POSITION | SET_SCALE | SET_ANGLE | SET_PIVOT_POINT);
	
	RegisterPerfFunc(&SpritePerfScreen::GameObjectRGBA888Draw, "GameObjectRGBA888Draw(position)", SET_POSITION);
	RegisterPerfFunc(&SpritePerfScreen::GameObjectRGBA888Draw, "GameObjectRGBA888Draw(position | pivot)", SET_POSITION | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::GameObjectRGBA888Draw, "GameObjectRGBA888Draw(position | angle)", SET_POSITION | SET_ANGLE);
	RegisterPerfFunc(&SpritePerfScreen::GameObjectRGBA888Draw, "GameObjectRGBA888Draw(position | scale)", SET_POSITION | SET_SCALE);
	RegisterPerfFunc(&SpritePerfScreen::GameObjectRGBA888Draw, "GameObjectRGBA888Draw(position | angle | pivot)", SET_POSITION | SET_ANGLE | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::GameObjectRGBA888Draw, "GameObjectRGBA888Draw(position | scale | pivot)", SET_POSITION | SET_SCALE | SET_PIVOT_POINT);
	RegisterPerfFunc(&SpritePerfScreen::GameObjectRGBA888Draw, "GameObjectRGBA888Draw(position | angle | scale)", SET_POSITION | SET_SCALE | SET_ANGLE);
	RegisterPerfFunc(&SpritePerfScreen::GameObjectRGBA888Draw, "GameObjectRGBA888Draw(position | angle | scale | pivot)", SET_POSITION | SET_SCALE | SET_ANGLE | SET_PIVOT_POINT);
	
	
	runIndex = 0;
	funcIndex = 0;
}

void SpritePerfScreen::UnloadResources()
{
	SafeRelease(redSprite);
	SafeRelease(greenSprite);
	SafeRelease(blueSprite);
	SafeRelease(tranclucentSprite);
	SafeRelease(zebraSprite);
	
	SafeRelease(redGameObject);
}

void SpritePerfScreen::WillAppear()
{
}

void SpritePerfScreen::WillDisappear()
{
	
}



void SpritePerfScreen::SpriteRGBA888Draw(PerfFuncData * data)
{
	int32 type = data->type;

	uint64 tm = SystemTimer::Instance()->AbsoluteMS();
	float x = 0;
	float y = 0;
	int vl = 0;

	redSprite->Reset();

	for (int i = 0; i < 10000; i++) 
	{
		if (type & SET_POSITION)
		{
			redSprite->SetPosition(x, y);
		}

		if (type & SET_ALL_DEFAULT_SETTERS)
		{
			redSprite->SetAngle(0.0f);
			redSprite->SetScale(1.0f, 1.0f);
			redSprite->SetPivotPoint(0.0f, 0.0f);
			redSprite->SetFrame(0);
		}

		if (type & SET_SCALE)
		{
			if (i & 1)
			{
				redSprite->SetScale(0.9f, 0.9f);
			}else {
				redSprite->SetScale(1.11f, 1.11f);
			}
		}

		if (type & SET_ANGLE)
			redSprite->SetAngle((float32)i);

		if (type & SET_PIVOT_POINT)
			redSprite->SetPivotPoint(10, 10);

		redSprite->Draw();
		x += 50;
		if(x >= 420)
		{
			x = 0;
			y += 50;
			if(y >= 320)
			{
				y = (float32)vl;
				vl++;
			}
		}
	}


	tm = SystemTimer::Instance()->AbsoluteMS() - tm;
	Logger::Info("%s, fill time = %lldms", data->name.c_str(), tm);
	SubmitTime(data, tm);
}

void SpritePerfScreen::SpriteRGBA888DrawStateDraw(PerfFuncData * data)
{
	int32 type = data->type;
	
	uint64 tm = SystemTimer::Instance()->AbsoluteMS();
	float x = 0;
	float y = 0;
	int vl = 0;
	
	redSprite->Reset();
	
	for (int i = 0; i < 10000; i++) 
	{
		redSpriteDrawState.position.x = 0;
		redSpriteDrawState.position.y = 0;
		redSpriteDrawState.scale.x = 1;
		redSpriteDrawState.scale.y = 1;
		redSpriteDrawState.pivotPoint.x = 0;
		redSpriteDrawState.pivotPoint.y = 0;
		redSpriteDrawState.angle = 0;
		redSpriteDrawState.flags = 0;

		
		if (type & SET_POSITION)
		{
			redSpriteDrawState.position.x = x;
			redSpriteDrawState.position.y = y;
		}
				
		if (type & SET_SCALE)
		{
			if (i & 1)
			{
				redSpriteDrawState.scale.x = 0.9f;
				redSpriteDrawState.scale.y = 0.9f;
			}else {
				redSpriteDrawState.scale.x = 1.11f;
				redSpriteDrawState.scale.y = 1.11f;
				//redSprite->SetScale(1.11f, 1.11f);
			}
		}
		
		if (type & SET_ANGLE)
		{
			redSpriteDrawState.angle = (float32)i;
		}
		if (type & SET_PIVOT_POINT)
		{
			redSpriteDrawState.pivotPoint.x = 10;
			redSpriteDrawState.pivotPoint.y = 10;
		}
		//redSprite->SetPivotPoint(10, 10);
		
		redSprite->Draw(&redSpriteDrawState);
		x += 50;
		if(x >= 420)
		{
			x = 0;
			y += 50;
			if(y >= 320)
			{
				y = (float32)vl;
				vl++;
			}
		}
	}
	
	
	tm = SystemTimer::Instance()->AbsoluteMS() - tm;
	Logger::Info("%s, fill time = %lldms", data->name.c_str(), tm);
	SubmitTime(data, tm);
}


void SpritePerfScreen::GameObjectRGBA888Draw(PerfFuncData * data)
{
	int32 type = data->type;
	
	float x = 0;
	float y = 0;
	int vl = 0;
	
	//redGameObject->Reset();
	Vector3 camera(0.0f, 0.0f, 0.0f);
	redGameObject->SetPosition(Vector2(0.0f, 0.0f));
	redGameObject->SetScale(Vector2(1.0f, 1.0f));
	redGameObject->SetPivotPoint(Vector2(0.0f, 0.0f));
	redGameObject->SetAngle(0.0f);
	
	uint64 tm = SystemTimer::Instance()->AbsoluteMS();
	for (int i = 0; i < 1000; i++) 
	{
		if (type & SET_POSITION)
		{
			//redGameObject->SetPosition(Vector3(x, y, 0));
			redGameObject->SetPosition(x, y);
			//redGameObject->localDrawState.position.x = x;
			//redGameObject->localDrawState.position.y = y;
		}
		
		if (type & SET_SCALE)
		{
			if (i & 1)
			{
				redGameObject->SetScale(Vector2(0.9f, 0.9f));
			}else {
				redGameObject->SetScale(Vector2(1.11f, 1.11f));
			}
		}
		
		if (type & SET_ANGLE)
			redGameObject->SetAngle((float32)i);
		
		if (type & SET_PIVOT_POINT)
			redGameObject->SetPivotPoint(Vector2(10, 10));
		
		redGameObject->Draw();
		x += 50;
		if(x >= 420)
		{
			x = 0;
			y += 50;
			if(y >= 320)
			{
				y = (float32)vl;
				vl++;
			}
		}
	}
	
	
	tm = SystemTimer::Instance()->AbsoluteMS() - tm;
	Logger::Info("%s, fill time = %lldms", data->name.c_str(), tm);
	SubmitTime(data, tm);
}


void SpritePerfScreen::SubmitTime(PerfFuncData * data, uint64 tm)
{
	data->runCount++;
	data->totalTime += tm;
	if (data->runCount == 1)
	{
		data->minTime = tm;
		data->maxTime = tm;
	}

	if (tm < data->minTime)
		data->minTime = tm;
	if (tm > data->maxTime)
		data->maxTime = tm;
}


void SpritePerfScreen::RegisterPerfFunc(void (SpritePerfScreen::*func)(PerfFuncData * data), const String & name, int32 type)
{
	PerfFuncData perfFuncData;
	perfFuncData.func = func;
	perfFuncData.name = name;
	perfFuncData.totalTime = 0;
	perfFuncData.runCount = 0;
	perfFuncData.minTime = 0;
	perfFuncData.maxTime = 0;
	perfFuncData.type = type;
	perfFuncs.push_back(perfFuncData);
}

void SpritePerfScreen::Input(UIEvent * touch)
{
	if ((touch->phase == UIEvent::PHASE_ENDED) && (touch->tid == UIEvent::BUTTON_1))
	{
		RestartTest();
	}
}

void SpritePerfScreen::Draw(const UIGeometricData &geometricData)
{
	if (funcIndex < (int32)perfFuncs.size())
	{
		const int32 RUN_COUNT = 30;
		if (runIndex < RUN_COUNT)
		{
			(this->*perfFuncs[funcIndex].func)(&perfFuncs[funcIndex]);
			runIndex++;
		}else if (runIndex == RUN_COUNT)
		{
			funcIndex++;
			runIndex = 0;
		}
	}
	else if (funcIndex == perfFuncs.size())
	{
		PrintLog();
		funcIndex++;
	}
}

void SpritePerfScreen::PrintLog()
{
	for (int32 k = 0; k < (int32)perfFuncs.size(); ++k)
	{
		Logger::Debug("%s: total: %lld min: %lld max: %lld avg: %lld",
					  perfFuncs[k].name.c_str(), 
					  perfFuncs[k].totalTime, 
					  perfFuncs[k].minTime,
					  perfFuncs[k].maxTime,
					  (uint64)perfFuncs[k].totalTime / (uint64)perfFuncs[k].runCount);
	}
}
