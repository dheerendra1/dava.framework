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
    DISCLAIMD. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#ifndef __DAVAENGINE_SPRITE_H__
#define __DAVAENGINE_SPRITE_H__

// TODO: убрать всю инициализацию объекта из конструктора и добавить в другое место которое сможет вернуть 0 в случае ошибки

#include "Base/BaseObject.h"
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/RenderBase.h"
#include "Render/Texture.h"
#include "Render/RenderDataObject.h"

namespace DAVA
{

enum eSpriteModification
{
	ESM_HFLIP = 1,
	ESM_VFLIP = 1<<1
};

class Texture;
	
/**
	\ingroup render_2d
	\brief Main class to draw 2d objects on screen.
	This object is fast and efficient for drawing 2d objects on screen. 
	It supports texture-atlassing and it uses texture atlasses automatically.
	You can set scale, rotate, pivot point, angle and position for any sprite object before rendering.
 */
class Sprite : public BaseObject 
{
public:
	struct DrawState
	{
		DrawState()
		{
			Reset();
		}
		
		Vector2 position;
		Vector2 pivotPoint;
		Vector2 scale;
		float32 angle;
		int32	frame;
		uint32  flags;
		float32 sinA;		
		float32 cosA;		
		float32 precomputedAngle;
		bool usePerPixelAccuracy;
		
		inline void Reset();
		inline void SetPosition(float32 x, float32 y);
		inline void SetPivotPoint(float32 x, float32 y);
		inline void SetScale(float32 x, float32 y);
		inline void SetAngle(float32 a);
		inline void SetFrame(uint32 frame);
		inline void SetFlags(uint32 flags);
		inline void SetPerPixelAccuracyUsage(bool needToUse);
		void BuildStateFromParentAndLocal(const Sprite::DrawState &parentState, const Sprite::DrawState &localState); 
	};
	
	
	
	enum eSpriteType 
	{
			SPRITE_FROM_FILE = 0
		,	SPRITE_FROM_TEXTURE
		,	SPRITE_RENDER_TARGET
	};
	
	enum eRectsAndOffsets 
	{
			X_POSITION_IN_TEXTURE = 0
		,	Y_POSITION_IN_TEXTURE
		,	ACTIVE_WIDTH
		,	ACTIVE_HEIGHT
		,	X_OFFSET_TO_ACTIVE
		,	Y_OFFSET_TO_ACTIVE
	};
	
	/**
	 \brief Function to create sprite. This is convinience function and it return sprite in any case.
	 If sprite is not available on disk it return 'purple' rect sprite. It will give ability for 
	 app to work stable and gave testers ability to find such errors fast.
	 
	 \param spriteName path to sprite name relative to application bundle
	 \param usedForScale is sprite will be used for scale
	 \return sprite pointer in any case will be returned
	 */	
	static Sprite* Create(const String &spriteName, bool usedForScale = false);// Creating sprite by name

	/**
	 \brief Function to create sprite as render target. 

	 \param sprWidth width of requested render target
	 \param sprHeight height of requested render target
	 \param textureFormat texture pixel format
	 \param contentScaleIncluded set true if content scale already taken into account. Just send false if you don't know how it's works.
	 
	 \return sprite pointer or 0 if it will be impossible to create such render target
	 */	
	static Sprite* CreateAsRenderTarget(float32 sprWidth, float32 sprHeight, Texture::PixelFormat textureFormat, bool contentScaleIncluded = false);
	void InitAsRenderTarget(float32 sprWidth, float32 sprHeight, Texture::PixelFormat textureFormat, bool contentScaleIncluded = false);
	
	/*
		\brief Function to create sprite
		\param spriteName path to sprite name
		\param forPointer you can create sprite in the allready allocated space or send NULL and memory will be allocated.
		\return 0 if sprite is unavailable and ptr to sprite if sprite is available
	 */	
	static Sprite* PureCreate(const String & spriteName, Sprite* forPointer = NULL); 

	
	/**
	 \brief Function to create sprite from the already created texture.
	 
	 \param fromTexture texture to create sprite from
	 \param xOffset horizontal offset in texture to the sprite
	 \param yOffset vertical offset in texture to the sprite
	 \param sprWidth width of requested sprite
	 \param sprHeight height of requested sprite
	 \param contentScaleIncluded set true if content scale allready taken into account. Just send false if you don't know how it's works.
	 
	 \return sprite pointer or 0 if it will be impossible to create such render target
	 */	
	static Sprite* CreateFromTexture(Texture *fromTexture, int32 xOffset, int32 yOffset, float32 sprWidth, float32 sprHeight, bool contentScaleIncluded = false);
	void InitFromTexture(Texture *fromTexture, int32 xOffset, int32 yOffset, float32 sprWidth, float32 sprHeight, bool contentScaleIncluded = false);


	void SetOffsetsForFrame(int frame, float32 xOff, float32 yOff);

	
	virtual int32 Release();
	
	Texture* GetTexture();
	Texture* GetTexture(int32 frameNumber);
	
	int32 GetFrameCount();
	
	float32 GetWidth();
	float32 GetHeight();
	const Vector2 &GetSize();
	
	const Vector2 &GetDefaultPivotPoint();
	
	void SetFrame(int32 frm);
	
	void SetDefaultPivotPoint(float32 x, float32 y);
	void SetDefaultPivotPoint(const Vector2 &newPivotPoint);
	
	void SetPivotPoint(float32 x, float32 y);
	void SetPivotPoint(const Vector2 &newPivotPoint);
	
	void SetPosition(float32 x, float32 y);
	void SetPosition(const Vector2 &drawPos);
	
	void SetAngle(float32 angleInRadians);
	
	void SetScale(float32 xScale, float32 yScale);
	void SetScale(const Vector2 &newScale);
	
	void SetScaleSize(float32 width, float32 height);//scale size overrides standart scale
	void SetScaleSize(const Vector2 &drawSize);
	
	void SetModification(int32 modif);
	
	void ResetPivotPoint();
	
	void ResetAngle();
	void ResetModification();
	void ResetScale();
	void Reset();//Reset do not resets the pivot point
	
	void Draw();	
	void Draw(DrawState * state);
	/** 
     \brief	Draw sprite by the 4 verticies. 
        The vertices sequence is (xLeft,yTop), (xRight,yTop), (xLeft,yBottom), (xRight,yBottom)
     \param v poiterto the array of the four Vector2 objects.
     */
	void DrawPoints(Vector2 *verticies);

    void DrawPoints(Vector2 *verticies, Vector2 *textureCoordinates);

	inline int32 GetResourceSizeIndex();

	
	const String GetName() { return relativePathname; }
	
	/** 
		 \brief	Return polygon for give frame in sprite. These polygons can be used for different purposes but the main purpose is 
			Easy export of collision detection polygons for the sprite-based objects
		 \param frame number of frame we looking a polygon for
		 \return ptr to polygon in case if it available, 0 in case if there is no poly for this sprite
	*/
	Polygon2 * GetPolygonForFrame(int32 frame);

	/** 
		\brief Function to get rect & offset of sprite frame position in texture
	 */
	float32 GetRectOffsetValueForFrame(int32 frame, eRectsAndOffsets valueType);

	/** 
		\brief Access to texCoords private field
	 */
	float32 ** GetTextureCoordinates() { return texCoords; }
	
	/**
		\brief Function to dump the sprites currently loaded in memory
			
	 */
	static void DumpSprites();
	
	/**
	 \brief Function prepares all sprites for the new screen size
	 
	 */
	static void ValidateForSize();

	/** 
		\brief Set polygon to draw the sprite with specific clip. Resets to null after draw.
	 */
	void SetClipPolygon(Polygon2 * clipPolygon);

	/** 
	 \brief Returns multiplyer to convert sprite to the physical coordinates.
	 */
	inline float32 GetResourceToPhysicalFactor();
    
	/** 
	 \brief Returns texture coordinates for the requested frame.
	 */
    float32 *GetTextureVerts(int32 frame);

	
protected:
	Sprite();
	Sprite(int32 sprWidth, int32 sprHeight, Texture::PixelFormat format);
	virtual ~Sprite();

	/** 
	 \brief Removes all sprite data.
	 */
	void Clear();
	
private:
    inline void PrepareSpriteRenderData(Sprite::DrawState * drawState);
    
	enum eSpriteTransform 
	{
		EST_ROTATE			= 1,
		EST_SCALE			= 1 << 1,
		EST_MODIFICATION	= 1 << 2
	};
	
	float32 tempVertices[8];

	
	String  relativePathname;
	
	Texture ** textures;
	int32 *frameTextureIndex;
	int32 textureCount;
	
	float32 **frameVertices;
	float32 **texCoords;

//	float32 **originalVertices;

	Polygon2 *polyArray;
	int32	*polyIndeces;
	int32	polygonsCount;

	Polygon2 * clipPolygon;
		
	void PreparePurplePlaceholder();
	
	void PrepareForNewSize();
	
	Vector2	size;
//	Vector2 originalSize;

	int32	frameCount;
	int32	frame;

	Vector2	defaultPivotPoint;
	Vector2	pivotPoint;
	
	Vector2	drawCoord;
	
	float32	rotateAngle;
	
	Vector2	scale;
	
	int32 modification;
	
	int32 flags;
	
	int32 resourceSizeIndex;

	float32 resourceToVirtualFactor;
	float32 resourceToPhysicalFactor;

	eSpriteType type;
	
//public:
	float32 **rectsAndOffsets;
    
    RenderDataStream * vertexStream;
    RenderDataStream * texCoordStream;
    ePrimitiveType primitiveToDraw;
    int32 vertexCount;
    
    // For rendering of clipped objects
    static Vector<Vector2> clippedTexCoords;
    static Vector<Vector2> clippedVertices;
    RenderDataObject * spriteRenderObject;
};
	
	
// inline functions implementation 
	
inline void Sprite::DrawState::Reset()
{
	position.x = 0.0f;
	position.y = 0.0f;
	pivotPoint.x = 0.0f;
	pivotPoint.y = 0.0f;
	scale.x = 1.0f;
	scale.y = 1.0f;
	angle = 0.0f;
	frame = 0;
	flags = 0;
	usePerPixelAccuracy = false;
	precomputedAngle = 0.0f;
	sinA = 0.0f;	// values for precomputed angle
	cosA = 1.0f;	// values for precomputed angle
}

inline void Sprite::DrawState::SetPosition(float32 x, float32 y)
{
	position.x = x;
	position.y = y;
}
	
inline void Sprite::DrawState::SetPivotPoint(float32 x, float32 y)
{
	pivotPoint.x = x;
	pivotPoint.y = y;
}
	
inline void Sprite::DrawState::SetScale(float32 x, float32 y)
{
	scale.x = x;
	scale.y = y;
}
	
inline void Sprite::DrawState::SetAngle(float32 a)
{
	angle = a;
}
	
inline void Sprite::DrawState::SetFrame(uint32 _frame)
{
	frame = _frame;
}

inline void Sprite::DrawState::SetFlags(uint32 _flags)
{
	flags = _flags;
}

inline void Sprite::DrawState::SetPerPixelAccuracyUsage(bool needToUse)
{
	usePerPixelAccuracy = needToUse;
}

	
inline int32 Sprite::GetResourceSizeIndex()
{
	return resourceSizeIndex;
}

inline float32 Sprite::GetResourceToPhysicalFactor()
{
	return resourceToPhysicalFactor;
}

	


};//end of namespace

#endif