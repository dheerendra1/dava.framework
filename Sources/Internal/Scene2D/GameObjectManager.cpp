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
#include "Scene2D/GameObjectManager.h"
#include "FileSystem/Logger.h"
#include "Collision/CollisionObject2.h"
#include "Render/RenderManager.h"

/*
	/TODO optimize object manager
	Optimize sort
	Optimize remove
*/

namespace DAVA
{
	
GameObjectManager::GameObjectManager() : cameraScale(1.f,1.f)
{
	isInUpdate = false;
}
	
GameObjectManager::~GameObjectManager()
{
	RemoveAllObjects();
}

void GameObjectManager::RemoveAllObjects()
{
	for (List<GameObject*>::iterator currentPos = objects.begin(); currentPos != objects.end(); ++currentPos)
	{
		GameObject * o = *currentPos;
		o->SetManager(0);
		SafeRelease(o);
	}
	objects.clear();
}
	
	
GameObjectManager * GameObjectManager::Create()
{
	GameObjectManager * manager = new GameObjectManager();
	return manager;
}

void GameObjectManager::MarkToChangeObjectPriority(GameObject * _object)
{
	if (isInUpdate)
	{
		changesStack.push(_object);
		_object->priorityChanged = true;
	}else
	{
		ChangeObjectPriority(_object);
	}
}
	
void GameObjectManager::ChangeObjectPriority(GameObject * _object)
{
	for (List<GameObject*>::iterator t = objects.begin(); t != objects.end(); ++t)
	{
		if (*t == _object)
		{
			objects.erase(t);
			break;
		}
	}
	
	List<GameObject*>::iterator currentPos = objects.begin();
	int pos = 0;
	for (; currentPos != objects.end(); ++currentPos)
	{
		GameObject * curObject = *currentPos;
			
		if ( curObject->GetPriority() > _object->GetPriority())
		{
			break;
		}
		pos++;
	}
	objects.insert(currentPos, _object);
}
	
void GameObjectManager::AddObject(GameObject * _object)
{
	/*if (isInUpdate)
	{
		changesStack.push(_object);
		_object->addedObject = true;
	}else
	{
		//if (object
		//_object->GetParent()->RecalcHierarchy(<#Sprite parentDrawState#>)*/
	RealAddObject(_object);

	List<GameObject*> & childs = _object->GetChildren();
	
	for (List<GameObject*>::iterator t = childs.begin(); t != childs.end(); ++t)
	{
		GameObject * child = *t;
		this->AddObject(child);
	}
	//}
}

void GameObjectManager::RealAddObject(GameObject * _object)
{
	if (!_object)return;
    
    if (_object->dead)  // if object is dead we mark that it should be added on deletion from prev. manager
    {
        _object->nextManager = this;
        return;
    }else
    {
        DVASSERT(_object->GetManager() == 0);
    }
    
	_object->Retain();
	
	//int objectsCount = objects.size();
	List<GameObject*>::iterator currentPos = objects.begin();
	
	//Logger::Debug("myp: %d\n", _object->GetPriority());
	int pos = 0;
	for (; currentPos != objects.end(); ++currentPos)
	{
		GameObject * curObject = *currentPos;
		if ( curObject->GetPriority() > _object->GetPriority())
		{
			//Logger::Debug("op: %d\n", curObject->GetPriority());
			
			break;
		}
		pos++;
	}
	//Logger::Debug("added to pos: %d\n", pos);
	objects.insert(currentPos, _object);
	_object->SetManager(this);
    RecalcObjectHierarchy(_object);
}

void GameObjectManager::RemoveObject(GameObject * _object)
{
	if (_object && _object->GetManager() == this)
	{
		_object->dead = true;
		
		changesStack.push(_object);
	}
}

bool PrioritySortFn(GameObject * o1, GameObject * o2)
{
	return o1->GetPriority() < o2->GetPriority();
}
	
/*
void GameObjectManager::SortObjects()
{
	std::sort(objects.begin(), objects.end(), PrioritySortFn);
}
*/

void GameObjectManager::RecalcObjectHierarchy(GameObject *object) const
{
    const GameObject *objParent = object->GetParent();
    object->RecalcHierarchy(objParent ? objParent->globalDrawState : drawState);
}
	
void GameObjectManager::RecalcObjectsHierarchy()
{
    List<GameObject*>::iterator currentPos_end = objects.end();
	for(List<GameObject*>::iterator currentPos = objects.begin(); currentPos != currentPos_end; ++currentPos)
	{
		GameObject * object = *currentPos;
		if (object->dead)continue;
		if (object->GetParent() == 0)
			object->RecalcHierarchy(drawState);
	}
}

void GameObjectManager::Update(float32 timeElapsed)
{
	isInUpdate = true;
	// Than perform usual update
	for (List<GameObject*>::iterator currentPos = objects.begin(); currentPos != objects.end(); ++currentPos)
	{
		GameObject *object = *currentPos;
		if (object->dead)continue;
		if(object->GetParent() == 0)
            object->Update(timeElapsed);
    }
	
	RecalcObjectsHierarchy();
	
	for (List<GameObject*>::iterator currentPos = objects.begin(); currentPos != objects.end(); ++currentPos)
	{
		GameObject * object = *currentPos;
		if (object->dead)continue;
		object->CollisionPhase();
	}		
	
	isInUpdate = false;
	
	ProcessChangesStack();
}
	
void GameObjectManager::ProcessChangesStack()
{
	while(changesStack.size() > 0)
	{
		GameObject * object = changesStack.top();
		changesStack.pop();
		if (object->priorityChanged)
		{
			ChangeObjectPriority(object);
			object->priorityChanged = false;
		}
        else if (object->dead)
		{
			// delete objects 
			for (List<GameObject*>::iterator t = objects.begin(); t != objects.end(); ++t)
			{
				if (*t == object)
				{
					objects.erase(t);
					break;
				}
			}
			object->dead = false;
			object->SetManager(0);
            
            if (object->nextManager)
            {
                GameObjectManager * nextManager = object->nextManager;
                object->nextManager = 0;
                object->SetManager(nextManager);
            }

			SafeRelease(object);			
		}
	}	
}

void GameObjectManager::Draw()
{
	eBlendMode srcMode = RenderManager::Instance()->GetSrcBlend();
	eBlendMode destMode = RenderManager::Instance()->GetDestBlend();
	
        RenderManager::Instance()->PushDrawMatrix();
        RenderManager::Instance()->SetDrawTranslate(cameraPosition);
        RenderManager::Instance()->SetDrawScale(cameraScale);

    const List<GameObject*>::iterator currentObjEnd = objects.end();
	for(List<GameObject*>::iterator currentObj = objects.begin(); currentObj != currentObjEnd; ++currentObj)
	{
		GameObject *object = *currentObj;
		if(object->dead)
            continue;
        else
		    object->Draw();
	}
	
        RenderManager::Instance()->PopDrawMatrix();

	RenderManager::Instance()->SetBlendMode(srcMode, destMode);
    RenderManager::Instance()->ResetColor();
}

void GameObjectManager::SetCameraPosition(float32 _cameraPositionX, float32 _cameraPositionY)
{
// 	drawState.position.x = -_cameraPositionX;
// 	drawState.position.y = -_cameraPositionY;
    cameraPosition.x = -_cameraPositionX;
    cameraPosition.y = -_cameraPositionY;
}
	
void GameObjectManager::SetCameraScale(float32 _cameraScale)
{
//	drawState.scale.x = drawState.scale.y = _cameraScale;
    cameraScale.x = cameraScale.y = _cameraScale;
}

float32 GameObjectManager::GetCameraScale() const
{
    return cameraScale.x; // > 0 ? drawState.scale.x : 1.f;
}
};