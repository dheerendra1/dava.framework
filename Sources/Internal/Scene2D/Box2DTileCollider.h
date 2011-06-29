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
#ifndef __DAVAENGINE_BOX2D_TILE_COLLIDER_H__
#define __DAVAENGINE_BOX2D_TILE_COLLIDER_H__

#include "Base/BaseMath.h"
#include "Base/BaseObject.h"
//#include "Render/2D/Sprite.h"
#include "Box2D/Box2D.h"



namespace DAVA
{
	

	class Box2DGameObjectManager;
	/**
	 \ingroup scene2d
	 \brief responce for collisions with Box2D objects and
	 */
	class Box2DTileCollider : public BaseObject
	{
		friend class Box2DGameObjectManager;
		
		struct TilesRangeBody
		{
			int32 from;
			int32 to;
			b2FixtureDef fixtureDef;
		};
	public:
		Box2DTileCollider(Box2DGameObjectManager *_manager, const Vector<int32> &_mapVector
						  , int32 _mapWidth, int32 _mapHeight
						  , int32 _tileWidth, int32 _tileHeight);
		
			///only static bodies can be added as the tiles
			///bodies adds in the centers of the tiles
		void RegisterBodyForTileIDs(const b2FixtureDef &fixtureDef, int32 idFrom, int32 idTo);

		b2Body *GetBodyForMapIndex(int32 mapIndex);
		b2Body *GetBodyForMapPos(int32 mapX, int32 mapY);

		void OnTileIDChanged(int32 mapIndex, int32 oldTileID);

		
	protected:
		virtual ~Box2DTileCollider();
		
		int32 mapWidth;
		int32 mapHeight;
		int32 tileWidth;
		int32 tileHeight;
		const Vector<int32> &mapVector;
		Vector<b2Body *> bodyVector;
		Vector<TilesRangeBody> rangesVector;
		Box2DGameObjectManager *manager;
		
	};
};
#endif
