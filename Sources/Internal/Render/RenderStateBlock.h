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
#ifndef __DAVAENGINE_RENDERSTATEBLOCK_H__
#define __DAVAENGINE_RENDERSTATEBLOCK_H__

#include "Render/RenderBase.h"
#include "Core/Core.h"

namespace DAVA
{


class BlendEnable
{
public:
    inline void UpdateVarGL(bool value)
    {
#if defined(__DAVAENGINE_OPENGL__)
        if (value)glEnable(GL_BLEND);
        else glDisable(GL_BLEND);
#elif defined(__DAVAENGINE_DIRECTX9__)
        
#endif
    }
    
};

class RenderStateBlock;
    
class RenderStateVariable
{
public:
    RenderStateVariable(RenderStateBlock * mainBlock);
    virtual ~RenderStateVariable();
    
    
    
};

template<class IMPL_CLASS, class V>
class RenderStateVariableImpl : public RenderStateVariable
{
public:
    RenderStateVariableImpl(RenderStateBlock * mainBlock);
    /*
    {
        mainBlock->varArray[mainBlock->varCount++] = this;
    }*/
    
    inline void UpdateVarGL() { renderStateImpl.UpdateVarGL(value); };
    inline void Set(const V & _value) { value = _value; };
                    
    V value;
    IMPL_CLASS renderStateImpl;
};

/**
    \brief Render state management base class
 
 */	

class RenderStateBlock
{
public:
    enum 
    {
        STATE_TYPE_UINT32 = 0,    
        STATE_TYPE_COLOR = 1,
    };
	enum
	{
        STATE_BLEND     = 0,
		STATE_BLENDFUNC = 1,  
        //        STATE_CULLFACE  = 1 << 2,     
        //        STATE_DEPTHTEST = 1 << 3,  

        STATE_ENABLE_TEXTURING = 2,    // Fixed func only
        STATE_COLOR = 3,  
	};
    enum
    {
        STATE_COUNT = 4
    };
    
    void Init(Core::eRenderer renderer);
    void Flush();
    
    
    typedef void (RenderStateBlock::*UpdateVarFunc)(uint32 value, uint32 oldValue);
    typedef void (RenderStateBlock::*UpdateVarFuncColor)(Color & value, Color & oldValue);

    void UpdateBlendGL(uint32 value, uint32 oldValue);
    void UpdateBlendDX(uint32 value, uint32 oldValue);
    
    void UpdateBlendFuncGL(uint32 func, uint32 oldValue);
    void UpdateBlendFuncDX(uint32 func, uint32 oldValue);

    void UpdateEnableTexturingGL(uint32 func, uint32 oldValue);
    void UpdateEnableTexturingDX(uint32 func, uint32 oldValue);
    
    void UpdateEnableColorGL(Color & value, Color & oldValue);
    void UpdateEnableColorDX(Color & value, Color & oldValue);

//    void UpdateCullFaceGL(uint32 value, uint32 oldValue);
//    void UpdateCullFaceDX(uint32 value, uint32 oldValue);

    void RegisterStateUInt32(uint32 state, UpdateVarFunc func);  
    void RegisterStateColor(uint32 state, UpdateVarFuncColor func);  
    
    
    
    inline void SetStateValue(uint32 state, uint32 value);
    inline uint32 GetStateValue(uint32 state); 
    
    
    
    uint32 activeStateSet;
    uint32 currentStateSet;
    
    uint32 stateType[STATE_COUNT];
    uint32 varValue[STATE_COUNT];
    Color varValueColor[STATE_COUNT];
    uint32 currentRendererValue[STATE_COUNT];
    Color currentRendererValueColor[STATE_COUNT];
    UpdateVarFunc varUpdateFuncArray[STATE_COUNT];
    UpdateVarFuncColor varUpdateFuncArrayColor[STATE_COUNT];
    uint32 varCount;
    
    
    bool isDebug;
};
    
    
inline void RenderStateBlock::SetStateValue(uint32 state, uint32 value)
{
    varValue[state] = value;
    currentStateSet |= (1 << state); // flag that value was changed
}

inline uint32 RenderStateBlock::GetStateValue(uint32 state) 
{
    return varValue[state];
}
    
};

#endif // __DAVAENGINE_RENDERSTATEBLOCK_H__
