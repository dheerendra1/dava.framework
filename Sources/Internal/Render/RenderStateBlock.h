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

/*
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
    
    inline void UpdateVarGL() { renderStateImpl.UpdateVarGL(value); };
    inline void Set(const V & _value) { value = _value; };
                    
    V value;
    IMPL_CLASS renderStateImpl;
};


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
    
*/
class Texture;
class Shader;
    
class RenderStateBlock
{
public:    
    enum
    {
        STATE_COLOR                 = 1 << 0, 
        STATE_BLEND_ENABLED         = 1 << 1,
        STATE_SRC_BLEND             = 1 << 2,
        STATE_DEST_BLEND            = 1 << 3, 
        STATE_BLEND_FUNC            = (1 << 2) | (1 << 3),
        STATE_TEXTURE_ENABLED       = 1 << 4,
        STATE_TEXTURE               = 1 << 5,
        STATE_DEPTH_TEST_ENABLED    = 1 << 6,
        STATE_DEPTH_WRITE_ENABLED   = 1 << 7,
        STATE_STENCIL_TEST_ENABLED  = 1 << 8,
//        STATE_ALPHA_TEST_ENABLED    = 1 << 9,
//        STATE_SCISSOR_TEST_ENABLED  = 1 << 10,
    };

    RenderStateBlock(Core::eRenderer _renderer);
    ~RenderStateBlock();
        
    Core::eRenderer renderer;
    uint32  state;
    Color   color;
    eBlendMode sourceFactor, destFactor;
    
    static const int32 MAX_TEXTURE_LEVELS = 2;
    Texture * currentTexture[MAX_TEXTURE_LEVELS];
    
    // STATE_COLOR
    inline void SetColor(float32 _r, float32 _g, float32 _b, float32 _a);
    inline void SetColor(const Color & _color);
    inline const Color & GetColor() const;
    inline void SetColorInHW();
    
    // STATE_BLEND_ENABLED
    inline void EnableBlending(bool isEnabled);
    inline bool IsBlendingEnabled(); 
    inline void SetEnableBlendingInHW();
    
    // STATE_BLEND_FUNC
    inline void SetBlendMode(eBlendMode _sourceFactor, eBlendMode _destFactor);
	inline eBlendMode GetSrcBlend();
	inline eBlendMode GetDestBlend();
    inline void SetBlendModeInHW();
    
    // STATE_TEXTURE_ENABLED
    inline void EnableTexturing(bool isEnabled);
    inline bool IsTexturingEnabled();
    inline void SetEnableTexturingInHW();
    
    // STATE_TEXTURE
    inline void SetTexture(Texture *texture, uint32 textureLevel = 0);
	inline Texture * GetTexture(uint32 textureLevel = 0);
    inline void SetTextureLevelInHW(uint32 textureLevel);
    
    // STATE_DEPTH_TEST_ENABLED
    inline void EnableDepthTest(bool isEnabled);
    inline bool IsDepthTestEnabled();
    inline void SetDepthTestInHW();

    // STATE_DEPTH_WRITE_ENABLED
    inline void EnableDepthWrite(bool isEnabled);
    inline bool IsDepthWriteEnabled();
    inline void SetDepthWriteInHW();
    
    
    /**
        Function to reset state to original zero state.
     */
    void Reset(bool doHardwareReset);
    
    /**
        Function to flush state into graphics hardware
        It checks what was changed from previous flush
     */
    void Flush(RenderStateBlock * previousState);
};

// Implementation of inline functions
inline void RenderStateBlock::SetColor(float32 _r, float32 _g, float32 _b, float32 _a)
{
    color.r = _r;
    color.g = _g;
    color.b = _b;
    color.a = _a;
}

inline void RenderStateBlock::SetColor(const Color & _color)
{
    color = _color;
}
    
inline const Color & RenderStateBlock::GetColor() const
{
    return color;
}

// STATE_BLEND_ENABLED
inline void RenderStateBlock::EnableBlending(bool isEnabled)
{
    if (isEnabled)state |= STATE_BLEND_ENABLED;
    else state &= ~STATE_BLEND_ENABLED;  
}
    
inline bool RenderStateBlock::IsBlendingEnabled()
{
    return (state & STATE_BLEND_ENABLED) != 0;
}

// STATE_BLEND_FUNC
inline void RenderStateBlock::SetBlendMode(eBlendMode _sourceFactor, eBlendMode _destFactor)
{
    sourceFactor = _sourceFactor;
    destFactor = _destFactor;
}
    
inline eBlendMode RenderStateBlock::GetSrcBlend()
{
    return sourceFactor;
}

inline eBlendMode RenderStateBlock::GetDestBlend()
{
    return destFactor;
}

// STATE_TEXTURE_ENABLED
inline void RenderStateBlock::EnableTexturing(bool isEnabled)
{
    if (isEnabled)state |= STATE_TEXTURE_ENABLED;
    else state &= ~STATE_TEXTURE_ENABLED;  
}

inline bool RenderStateBlock::IsTexturingEnabled()
{
    return (state & STATE_TEXTURE_ENABLED) != 0;
}

// STATE_TEXTURE
inline void RenderStateBlock::SetTexture(Texture *texture, uint32 textureLevel)
{
    currentTexture[textureLevel] = texture;
}

inline Texture * RenderStateBlock::GetTexture(uint32 textureLevel)
{
    return currentTexture[textureLevel];
}

// STATE_DEPTH_TEST_ENABLED
inline void RenderStateBlock::EnableDepthTest(bool isEnabled)
{
    if (isEnabled)state |= STATE_DEPTH_TEST_ENABLED;
    else state &= ~STATE_DEPTH_TEST_ENABLED; 
}

inline bool RenderStateBlock::IsDepthTestEnabled()
{
    return (state & STATE_DEPTH_TEST_ENABLED) != 0;
}

// STATE_DEPTH_WRITE_ENABLED
inline void RenderStateBlock::EnableDepthWrite(bool isEnabled)
{
    if (isEnabled)state |= STATE_DEPTH_WRITE_ENABLED;
    else state &= STATE_DEPTH_WRITE_ENABLED; 
}

inline bool RenderStateBlock::IsDepthWriteEnabled()
{
    return (state & STATE_DEPTH_WRITE_ENABLED) != 0;
}
    

    
};

#endif // __DAVAENGINE_RENDERSTATEBLOCK_H__
