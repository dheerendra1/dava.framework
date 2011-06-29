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
#include "Scene3D/SkeletonNode.h"
#include "Render/RenderHelper.h"

namespace DAVA 
{

SkeletonNode::SkeletonNode(Scene * _scene)
	: BoneNode(_scene, 0)
{
}

SkeletonNode::~SkeletonNode()
{
	
}
	
void SkeletonNode::EnableDebugDraw(bool _enableDebugDraw)
{
	enableDebugDraw = _enableDebugDraw;
}

void SkeletonNode::Update(float32 timeElapsed)
{
	BoneNode::Update(timeElapsed);

//	//if (isMatrixesChanged)
//	{
//		if (parent)
//			worldTransform = localTransform * parent->worldTransform;
//		else 
//			worldTransform = localTransform;
//		isMatrixesChanged = false;
//	}
//	//printf("- node: %s tr: %f %f %f\n", name.c_str(), localTransform.data[12], localTransform.data[13], localTransform.data[14]); 
//	
//	uint32 size = (uint32)childs.size();
//	for (uint32 c = 0; c < size; ++c)
//		childs[c]->Update(timeElapsed);
}

void SkeletonNode::Draw()
{
	BoneNode::Draw();
//
//	glPushMatrix();
//	glMultMatrixf(worldTransform.data);
//	
//	glDisable(GL_TEXTURE_2D);
//	
//	glColor4f(0.984375, 0.078125, 0.64453125, 1.0f);
//		//glutWireCube(0.5f);
//		
//	AABBox box(Vector3(0.0f, 0.0f, 0.0f), 5.0f);
//	RenderHelper::DrawBox(box);
//	
//	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//	glPopMatrix();
//	glEnable(GL_TEXTURE_2D);
}

};