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
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#ifndef __SPRITE_NODE_H__
#define __SPRITE_NODE_H__

#include "Scene3D/SceneNode3d.h"
#include "Render/RenderDataObject.h"

namespace DAVA 
{
class Sprite;
    
/**
    \ingroup 
    \brief 
 */
class SpriteNode : public SceneNode
{
public:
    
    SpriteNode(Scene * _scene, const String &pathToSprite, int32 frame = 0
               , const Vector2 &reqScale = Vector2(1.0f, 1.0f)
               , const Vector2 &pivotPoint = Vector2(0.0f, 0.0f));
    SpriteNode(Scene * _scene, Sprite *spr, int32 frame = 0
               , const Vector2 &reqScale = Vector2(1.0f, 1.0f)
               , const Vector2 &pivotPoint = Vector2(0.0f, 0.0f));
    ~SpriteNode();
    
    enum eType
    {
        TYPE_OBJECT = 0,            //! draw sprite without any transformations. Set by default.
        TYPE_BILLBOARD,             //! normal billboard when billboard is always parallel to the camera projection plane. It computed by multiplication of worldMatrix of node to [R]^-1 matrix of camera
        TYPE_BILLBOARD_TO_CAMERA,   //! billboard is facing to camera point
    };
    
    /**
        \brief Set type of coordinates modification for the given sprite node
        \param[in] type type you want to set
     */
    void SetType(eType type);
    /**
        \brief Get type of coordinates modification for the given sprite node
        \returns type that was set to this sprite node
     */
    eType GetType();
    
    /**
        \brief Overriden draw function that draws sprite
     */
    virtual void	Draw();
    
    /**
        \brief Change sprite frame for this sprite node 
        \param[in] newFrame frame you want to set
     */
    void SetFrame(int32 newFrame);
    
    /**
        \brief Get frame for this sprite node
        \returns frame index that was set for this node last time
     */
    int32 GetFrame();
protected:
    
    void CreateMeshFromSprite(int32 frameToGen);
    Vector<float32> verts;
    Vector<float32> textures;
    
    Sprite *sprite;
    Vector2 sprScale;
    Vector2 sprPivot;
    int32 frame;
    
    RenderDataObject *renderData;
    
    eType type;
};
};

#endif