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
#include "Scene3D/RotatingCubeNode.h"

namespace DAVA
{

RotatingCubeNode::RotatingCubeNode(Scene * _scene)
	: SceneNode(_scene)
{
	SetupCube();
}

RotatingCubeNode::~RotatingCubeNode()
{

}

void RotatingCubeNode::SetupCube()
{
	float one = 1.0f;
	float vertices[] = {
		-one, -one, -one,
		one, -one, -one,
		one,  one, -one,
		-one,  one, -one,
		-one, -one,  one,
		one, -one,  one,
		one,  one,  one,
		-one,  one,  one,
	};
	
	int colors[] = {
		0	,	0,    0,  255,
		255	,   0,    0,  255,
		255	, 255,    0,  255,
		0	, 255,    0,  255,
		0	,	0,  255,  255,
		255	,   0,  255,  255,
		255	, 255,  255,  255,
		0	, 255,  255,  255,
	};
	
	int indices[] = 
	{
		0, 4, 5,    0, 5, 1,
		1, 5, 6,    1, 6, 2,
		2, 6, 7,    2, 7, 3,
		3, 7, 4,    3, 4, 0,
		4, 7, 6,    4, 6, 5,
		3, 0, 1,    3, 1, 2
	};
	
	
	cube = new PolygonGroup();
	cube->AllocateData( EVF_COORD | EVF_COLOR, 12, 36, 0);

	for (int i = 0; i < 8 ; ++i)
	{
		cube->SetCoord(i, Vector3(vertices[i * 3 + 0], vertices[i * 3 + 1], vertices[i * 3 + 2]));
		cube->SetColor(i, RGBColor(colors[i * 3 + 0], colors[i * 3 + 1], colors[i * 3 + 2]));
	}
	for (int i = 0; i < 36; ++i)
	{				   
		cube->SetIndex(i, indices[i]);
	}
	
}

void RotatingCubeNode::Update(float32 timeElapsed)
{

	SceneNode::Update(timeElapsed);
}
	
void RotatingCubeNode::Draw()
{
//	printf("Render\n");
	glPushMatrix();
	
	
//	glDisable(GL_LIGHTING);
	
//	glColor4f(1.0f, 1.0f, 0.0f, 255.0f);
	
	glVertexPointer(3, GL_FLOAT, cube->vertexStride, cube->vertexArray);
//	glNormalPointer(GL_FLOAT, cube->vertexStride, cube->normalArray);
	glColorPointer(4, GL_UNSIGNED_BYTE, cube->vertexStride, cube->colorArray);
//	glTexCoordPointer(2, GL_FLOAT, cube->vertexStride, cube->textureCoordArray[0]);
	
	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawElements(GL_TRIANGLES, cube->indexCount, GL_UNSIGNED_SHORT, cube->indexArray);
	
	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glPopMatrix();
	SceneNode::Draw();
}

};