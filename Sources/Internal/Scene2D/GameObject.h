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
#ifndef __DAVAENGINE_GAME_OBJECT_H__
#define __DAVAENGINE_GAME_OBJECT_H__

#include "Base/BaseMath.h"
#include "Render/2D/Sprite.h"
#include "Animation/AnimatedObject.h"
#include "Animation/Interpolation.h"

//#define DRAW_COLLISIONS

/**
	\defgroup scene2d 2D Scene Management
 */

namespace DAVA
{
	

class GameObjectManager;
class CollisionObject2;
/**
	\ingroup scene2d
	\brief base class in 2d scene hierarchy
 */
class GameObject : public AnimatedObject
{
protected:
	virtual ~GameObject();
	GameObject();

	Sprite::DrawState	localDrawState;
	Sprite::DrawState	globalDrawState;
	Color				color;
	eBlendMode			srcOp;
	eBlendMode			destOp;
	
	struct
	{
		uint32		groupId: 28;
		uint8		visible : 1;
		uint8		dead : 1;
		uint8		priorityChanged : 1;
		uint8		addedObject: 1;		// can be used for delayed adding of anything to hierarchy
	};

public:
	// public properties of the object
	
	
	
	static GameObject	* Create(const String & _pathToSprite, int32 frame = 0);
	static GameObject	* Create(Sprite * sprite, int32 frame = 0);
	
	inline void		SetPosition(const Vector2 & position);
	inline void		SetPosition(float32 x, float32 y);
	inline Vector2 & GetPosition();

	inline void		SetScale(const Vector2 & scale);
	inline void		SetScale(float32 x, float32 y);
	inline Vector2 & GetScale();
	
		   void		SetPivotPoint(int32 alignFlags);
	inline void		SetPivotPoint(const Vector2 &newPivotPoint);
	inline void		SetPivotPoint(float32 x, float32 y);
	inline Vector2 & GetPivotPoint();
	
	inline Vector2	GetSize();
	
	inline void		SetSprite(Sprite * sprite);
	inline void		SetSprite(const String &spriteName);
	inline Sprite	* GetSprite();

	/**
		\brief Function changes priority of object. 
		This function do not change priorities for childs, so if you want to make logic that allow you 
		to change priority of childs according to some rules you should overload this function. 
		\param[in] newPriority new priority to be set
	 */
	virtual void	SetPriority(int32 newPriority);
	inline int		GetPriority();
	
	void			SetFrame(int32 frame);
	inline int32	GetFrame() const;
	
	inline void		SetGroupId(int32 frame);
	inline int32	GetGroupId();
	
	inline void	SetAngle(float32 angleInRadians);
	inline float32	GetAngle();
	
	inline void		SetColor(float32 r, float32 g, float32 b, float32 a);
	inline void		SetColor(const Color & _color);
	inline Color &	GetColor(); 
    inline void		SetBlendMode(eBlendMode	srcOp, eBlendMode destOp);
	
    inline bool GetVisible();
	inline void SetVisible(bool isVisible);

	inline bool IsDead();

	inline void SetUserData(void * userData);
	inline void * GetUserData() const;
	
	inline const Sprite::DrawState & GetGlobalDrawState() const;
	inline const Sprite::DrawState & GetLocalDrawState() const;
	
	
	void			ChangeManager(GameObjectManager * newManager);
	inline	GameObjectManager * GetManager();
	
	
	/**
		\brief checks if current gameobject collides with another game object
		This function checks collision using CollisionObject2 object. By default 
		there is no collision object inside GameObject, so you need to create it manually and set it
		using \ref SetCollisionObject function.
		In case if any of both game objects do not have collision object set, collision function return false.
	 
		You should call this function inside GameObject::CollisionPhase callback. It guaranties that all object 
		properties will be recalculated and passed to CollisionObject2 object. 
	 
		\param[in] gameObject object to test collision with
		\returns true if collision occurs, false if there is no collision or one of objects do not have collision object set
	 */
	bool			IsCollideWith(GameObject * gameObject);
    bool            IsCollideWith(CollisionObject2 * collision2);
	void			SetCollisionObject(CollisionObject2 * obj);
	void			BuildCollisionObjectFromSpritePoly(int32 frame);
	inline			CollisionObject2 * GetCollision();

	void			SetDebugDraw(bool _isDebugDraw); 
	/*
	 
	 */
	Animation *		WaitAnimation(float32 time, int32 track = 0);
	Animation *		MoveAnimation(float32 x, float32 y, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	
	inline Animation *		PositionAnimation(DAVA::Vector2 v, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0)
	{
		return MoveAnimation(v.x, v.y, time, interpolationFunc, track);
	}
	
	Animation *		MoveBySplineAnimation(BezierSpline2 * spline, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	Animation *		ScaleAnimation(const Vector2 & scale, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	Animation *		FrameInterpolateAnimation(int32 startFrame, int32 endFrame, float32 time, int32 track = 0);
	Animation *		ColorAnimation(const Color & color, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	Animation *		RotateAnimation(float32 newAngle, float32 time, Interpolation::FuncType interpolationFunc = Interpolation::LINEAR, int32 track = 0);
	Animation *		VisibleAnimation(bool visible, int32 track = 0);
    
	Animation *			RemoveFromManagerAnimation(int32 track = 0);
	
	/**
		\brief This function is called every frame to let you update objects in hierarchy
		It called every frame and all you get is time elapsed. 
		When you develop game try to make all animations / movements and everything that is related to game attached to time.
		So if you animating the object instead of using object->x += 10; use object->x += timeElapsed * 10; that means 10 pixels per second. 
		
		\code
		void Rocket::Update(float32 timeElapsed)
		{
			// update your information like position, frame, priority for the current frame drawn. 	 
	 
	 
			// call GameObject::Update to update all your childs. 
			//	You can be sure that parent is already updated if you are in child update function. 
			GameObject::Update(timeElapsed);
		}
		\endcode
	 
		\param[in] timeElapsed time from previous frame
	 */
	virtual void	Update(float32 timeElapsed);
	/**
		\brief This function process whole hierarchy, and compute world positions for every object
		It called every frame after update and process whole hierarchy
		\param[in] parentDrawState pass the calculated parentDrawState 
	 */
	virtual void	RecalcHierarchy(Sprite::DrawState & parentDrawState);
	/**
		\brief Here you can check all collisions and make all collision responses. 
		If you'll update position, scale or rotate in this function you should manually call RecalcHierarchy after that 
		for this particular node.
	 */
	virtual void	CollisionPhase();
	/**
		\brief Function that draws object
		If you want to implement custom drawing for gameobjects you should override this function
	 */
	virtual void	Draw();
	
	/**
		\brief Function adds object as child to this object, but it's assume that manager already set for this object.
		This function can be used in situations when you have some object in manager and you decided to attach it 
		to another object suddenly.
	 
		\param[in] gameObject object you want to attach to
	 */
	virtual void	AttachObject(GameObject * gameObject);
	/**
		\brief Function removes object from this object, but do not remove it from manager
		This is required when you want to remove some object, but leave childs in scene, for example, 
		you want to leave particle emitters of the object in scene. 
		
		\param[in] gameObject object you want to detach
	 */
	virtual void	DetachObject(GameObject * gameObject);

    /**
         \brief Function adds object as child to this object, and insert it into the object manager.
         \param[in] gameObject object you want to add
	 */
	virtual void	AddObject(GameObject * gameObject);
	
    /**
        \brief Function removes object from this gameObject and from it's object manager hierarchy.
        \param[in] gameObject object you want to remove
	 */
	virtual void	RemoveObject(GameObject * gameObject);

	virtual GameObject * GetParent() const;
	
	List<GameObject*> & GetChildren();
protected:
	Sprite *	sprite;
	int32		priority;
	
	GameObjectManager * manager;
    /*
        This is required for cases when you want to delete object from one manager and add to another manager on the same frame
     */
    GameObjectManager * nextManager;    
	virtual void SetManager(GameObjectManager * _manager);

	List<GameObject*> children;
	GameObject * parent;

	void * userData;
	// collision information
	CollisionObject2 * collision;
#if defined(__DAVAENGINE_DEBUG__)
	bool isDebugDraw;
#endif
	
	// callbacks for the visible & remove animations
    void VisibleAnimationCallback(BaseObject * caller, void * param, void *callerData);
	void RemoveFromManagerAnimation(BaseObject * animation, void * userData, void *callerData);

	friend class GameObjectManager;	
};
	
	
// Implementation of inline functions
inline void	GameObject::SetSprite(Sprite * _sprite)
{
	SafeRelease(sprite);
	sprite = _sprite;
	if (sprite)sprite->Retain();
	SetFrame(0);
}
	
inline void GameObject::SetSprite(const String &spriteName)
{
	Sprite *spr = Sprite::Create(spriteName);
	SetSprite(spr);
	SafeRelease(spr);
}

	
inline Sprite * GameObject::GetSprite()
{
	return sprite;
}

inline int GameObject::GetPriority()
{
	return priority;
}


inline int32 GameObject::GetFrame() const
{
	return localDrawState.frame;
}

inline void		GameObject::SetGroupId(int32 _groupId) 
{ 
	groupId = _groupId; 
};

inline int32	GameObject::GetGroupId() 
{ 
	return groupId; 
}


inline void GameObject::SetPivotPoint(const Vector2 &newPivotPoint)
{
	localDrawState.pivotPoint = newPivotPoint;
}

inline void GameObject::SetPivotPoint(float32 x, float32 y)
{
	localDrawState.pivotPoint.x = x;
	localDrawState.pivotPoint.y = y;
}
	
inline Vector2 & GameObject::GetPivotPoint()
{
	return localDrawState.pivotPoint;
}
	
inline Vector2	GameObject::GetSize()
{
	return Vector2(sprite->GetWidth() * localDrawState.scale.x, sprite->GetHeight() * localDrawState.scale.y);
}

	
inline void	GameObject::SetPosition(const Vector2 & _position)
{
	localDrawState.position = _position;
}
	
inline void	GameObject::SetPosition(float32 x, float32 y)
{
	localDrawState.position.x = x;
	localDrawState.position.y = y;
}
	
inline Vector2 & GameObject::GetPosition()
{
	return localDrawState.position;
}
	
inline void	 GameObject::SetScale(const Vector2 & _scale)
{
	localDrawState.scale = _scale;
}
	
inline void	 GameObject::SetScale(float32 x, float32 y)
{
	localDrawState.scale.x = x;
	localDrawState.scale.y = y;
}
	
inline Vector2 & GameObject::GetScale() 
{
	return localDrawState.scale;
}
	
inline	CollisionObject2 * GameObject::GetCollision()
{
	return collision;
}
	
inline	GameObjectManager * GameObject::GetManager()
{
	return manager;
}

inline void		GameObject::SetAngle(float32 angleInRadians)
{
	localDrawState.angle = angleInRadians;
}

inline float32	GameObject::GetAngle()
{
	return localDrawState.angle;
}

inline void	GameObject::SetColor(float32 r, float32 g, float32 b, float32 a)
{
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}
	
inline void	GameObject::SetColor(const Color & _color)
{
	color = _color;
}
	
inline Color & GameObject::GetColor()
{
	return color;
}

inline void GameObject::SetBlendMode(eBlendMode	_srcOp, eBlendMode _destOp)
{
	srcOp = _srcOp;
	destOp = _destOp;
}

inline bool GameObject::GetVisible()
{
    return visible;
}

inline void GameObject::SetVisible(bool isVisible)
{
    visible = isVisible;
}
	
void GameObject::SetUserData(void * _userData)
{
	userData = _userData;
}

void * GameObject::GetUserData() const
{
	return userData;
}

inline bool GameObject::IsDead()
{
	return dead;	
}
	
inline const Sprite::DrawState & GameObject::GetGlobalDrawState() const
{
	return globalDrawState;
}

inline const Sprite::DrawState & GameObject::GetLocalDrawState() const
{
	return localDrawState;
}

inline void GameObject::SetDebugDraw(bool _isDebugDraw)
{
#if defined(__DAVAENGINE_DEBUG__) 
	isDebugDraw = _isDebugDraw; 
#endif // __DAVAENGINE_DEBUG__
}

};

#endif 