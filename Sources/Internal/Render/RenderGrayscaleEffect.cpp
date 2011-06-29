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
#include "Render/RenderGrayscaleEffect.h"
#include "Render/RenderManager.h"

namespace DAVA 
{
#if defined(__DAVAENGINE_OPENGL__)
void RenderGrayscaleEffect::StartEffect()
{

	glColor4f(0.5f + 0.3f * RenderManager::Instance()->GetColorR(), 0.5f + 59.0f  * RenderManager::Instance()->GetColorG(), 0.5f + 0.11f  * RenderManager::Instance()->GetColorB(), RenderManager::Instance()->GetColorA());
	
	float constColor[] = {0.67f, 0.67f, 0.67f, 0.25f};
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RenderManager::Instance()->GetTexture()->id);
	
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constColor);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
	
	
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, RenderManager::Instance()->GetTexture()->id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
	
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
	
}

void RenderGrayscaleEffect::StopEffect()
{
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RenderManager::Instance()->GetTexture()->id);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	float constColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constColor);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void RenderGrayscaleEffect::SetColor(float r, float g, float b, float a)
{
	glColor4f(0.5f + 0.3f * r, 0.5f + 59.0f  * g, 0.5f + 0.11f  * b, a);
}

void RenderGrayscaleEffect::SetTexture(Texture *texture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id);

}
	
	void RenderGrayscaleEffect::SetVertexPointer(int size, int type, int stride, const void *pointer)
	{
		glVertexPointer(size, type, stride, pointer);
	}
	

void RenderGrayscaleEffect::SetTexCoordPointer(int size, int type, int stride, const void *pointer)
{
	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(size, type, stride, pointer);
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(size, type, stride, pointer);
}
#elif defined(__DAVAENGINE_DIRECTX9__)


#endif

};

