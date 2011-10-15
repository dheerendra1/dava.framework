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
#include "Scene2D/Box2DTileCollider.h"
#include "Scene2D/Box2DGameObjectManager.h"

namespace DAVA
{
	Box2DTileCollider::Box2DTileCollider(Box2DGameObjectManager *_manager, const Vector<int32> &_mapVector
										 , int32 _mapWidth, int32 _mapHeight
										 , int32 _tileWidth, int32 _tileHeight)
	: BaseObject()
	, mapVector(_mapVector)
	
	{
		mapWidth = _mapWidth;
		mapHeight = _mapHeight;
//		mapVector = _mapVector;
//		_mapVector.pop_back();
		tileWidth = _tileWidth;
		tileHeight = _tileHeight;
		manager = _manager;

		b2ContactManager &contactManager = (b2ContactManager &)manager->box2DWorld->GetContactManager();
		contactManager.m_broadPhase.CreateTiledMap(manager->VectorGameManagerToBox2D(Vector2(0, (float32)mapHeight * tileHeight))
												   , manager->SizeGameManagerToBox2D(Vector2((float32)tileWidth, (float32)tileHeight))
												   , mapWidth, mapHeight);
		bodyVector.resize(mapWidth * mapHeight, NULL);

		manager->SetTileCollider(this);

	}
	
	Box2DTileCollider::~Box2DTileCollider()
	{
			//TODO: removes regions on destructor
	}
	
	b2Body *Box2DTileCollider::GetBodyForMapIndex(int32 mapIndex)
	{
		return bodyVector[mapIndex];
	}
	
	b2Body *Box2DTileCollider::GetBodyForMapPos(int32 mapX, int32 mapY)
	{
		return GetBodyForMapIndex(mapX + mapY * mapWidth);
	}

	void Box2DTileCollider::OnTileIDChanged(int32 mapIndex, int32 oldTileID)
	{
		int32 oldR = -1;
		int32 newR = -1;
		int32 newID = mapVector[mapIndex];
		for (int32 i = 0; i < (int32)rangesVector.size(); i++) 
		{
			if (oldTileID >= rangesVector[i].from && oldTileID <= rangesVector[i].to)
			{
				oldR = i;
			}
			if (newID >= rangesVector[i].from && newID <= rangesVector[i].to) 
			{
				newR = i;
			}
		}
		if (newR == oldR) 
		{
			return;
		}
		
		if (bodyVector[mapIndex])
		{
			manager->box2DWorld->DestroyBody(bodyVector[mapIndex]);
			bodyVector[mapIndex] = NULL;
		}
		if (newR >= 0) 
		{
			b2BodyDef bDef;
			int32 mapY = mapIndex / mapWidth;
			int32 mapX = mapIndex -  mapY * mapWidth;
					
			float32 posX = (float32)(mapX * tileWidth + (tileWidth >> 1));
			float32 posY = (float32)(mapY * tileHeight + (tileHeight >> 1));
					
			bDef.position = manager->VectorGameManagerToBox2D(Vector2(posX, posY));
					
			b2Body *tileBody = manager->box2DWorld->CreateBody(&bDef);
			bodyVector[mapIndex] = tileBody;
			tileBody->SetTiled(true);
			tileBody->SetUserData((void *)mapIndex);
					
			tileBody->CreateFixture(&(rangesVector[newR].fixtureDef));
		}
	}


	
	void Box2DTileCollider::RegisterBodyForTileIDs(const b2FixtureDef &fixtureDef, int32 idFrom, int32 idTo)
	{
		TilesRangeBody reg;
		reg.from = idFrom;
		reg.to = idTo;
		reg.fixtureDef = fixtureDef;
			//this fucking code here is because of the box2d author hands
		b2Shape *sh;
		switch (fixtureDef.shape->GetType())
		{
			case b2Shape::e_circle:
				sh = new b2CircleShape();
				*sh = *fixtureDef.shape;
				break;
			case b2Shape::e_edge:
				sh = new b2EdgeShape();
				*sh = *fixtureDef.shape;
				break;
			case b2Shape::e_chain:
				sh = new b2ChainShape();
				*sh = *fixtureDef.shape;
				break;
			case b2Shape::e_polygon:
				sh = new b2PolygonShape();
				*sh = *fixtureDef.shape;
				break;
		}
		reg.fixtureDef.shape = sh;
		rangesVector.push_back(reg);
		
		b2BodyDef bDef;
		for (int32 i = 0; i < (int32)mapVector.size(); i++) 
		{
			if (mapVector[i] >= idFrom && mapVector[i] <= idTo)
			{
				int32 mapY = i / mapWidth;
				int32 mapX = i -  mapY * mapWidth;
				
				float32 posX = (float32)(mapX * tileWidth + (tileWidth >> 1));
				float32 posY = (float32)(mapY * tileHeight + (tileHeight >> 1));
				
				bDef.position = manager->VectorGameManagerToBox2D(Vector2(posX, posY));
				
				b2Body *tileBody = manager->box2DWorld->CreateBody(&bDef);
				if (bodyVector[i])
				{
					manager->box2DWorld->DestroyBody(bodyVector[i]);
				}
				bodyVector[i] = tileBody;
				tileBody->SetTiled(true);
				tileBody->SetUserData((void *)i);
				
				tileBody->CreateFixture(&fixtureDef);
			}
		}
		
	}
	
};
