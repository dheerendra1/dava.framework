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
#ifndef __DAVAENGINE_RENDERMANAGER_H__
#define __DAVAENGINE_RENDERMANAGER_H__

#include "Render/RenderBase.h"
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Render/2D/Sprite.h"
#include "Platform/Mutex.h"
#include "Platform/Thread.h"
#include "Render/RenderEffect.h"
#include "Core/DisplayMode.h"
#include "Core/Core.h"
#include "Render/Cursor.h"

#include <stack>

namespace DAVA
{

	
class Texture;
class Shader;
#if defined(__DAVAENGINE_DIRECTX9__)
//#include "D3DInitialize.h"
#endif // __DAVAENGINE_DIRECTX9__

/** 
	\ingroup render
	\brief Main class that implements rendering abstraction layer.
	
	If you want to write portable code you'll need to use this class and other classes that rely on our RenderManager. 
	Do not use native drawing functions, to avoid portability problems in the future. 
 */
class RenderManager : public Singleton<RenderManager>
{
public:
    static RenderEffect * FLAT_COLOR;
    static RenderEffect * TEXTURE_MUL_FLAT_COLOR;
    static RenderEffect * TEXTURE_MUL_FLAT_COLOR_ALPHA_TEST;
    
    
    struct Caps
	{
        Core::eRenderer renderer;
		bool            isHardwareCursorSupported;
	};
    
    struct Stats
    {
        void Clear();
        
        uint32 drawArraysCalls;
        uint32 drawElementsCalls;
        uint32 primitiveCount[PRIMITIVETYPE_COUNT];
    };
    
    static void Create(Core::eRenderer renderer);

	RenderManager(Core::eRenderer renderer);
	virtual ~RenderManager();
    
    
    Core::eRenderer GetRenderer() { return renderer; };

	/** 
	 \brief Inits
	 \param[in] orientation
	 \param[in] screenWidth
	 \param[in] screenHeight
	 */
#ifdef __DAVAENGINE_OPENGL__ 
	void Init(int32 _frameBufferWidth, int32 _frameBufferHeight);
#else
	void Init(int32 _frameBufferWidth, int32 _frameBufferHeight);
	LPDIRECT3D9		  GetD3D();
	LPDIRECT3DDEVICE9 GetD3DDevice();
	void SetupDefaultDeviceState();
	void OnDeviceLost();
	void OnDeviceRestore();
	HWND hWnd;
	HINSTANCE hInstance;
	D3DPRESENT_PARAMETERS	presentParams;

	LPDIRECT3DSURFACE9 backBufferSurface;
	LPDIRECT3DSURFACE9 depthStencilSurface;

#endif 

#ifdef __DAVAENGINE_WIN32__
	bool Create(HINSTANCE hInstance, HWND hWnd);
#else
	bool Create();
#endif 
	bool ChangeDisplayMode(DisplayMode mode, bool isFullscreen);
	void Invalidate();

	void Release();
	
	bool IsDeviceLost();
	void BeginFrame();
	void EndFrame();	
	bool IsInsideDraw(); // function returns true if we are between calls to BeginFrame & EndFrame

	// RenderManager capabilities & performance statistics
	void DetectRenderingCapabilities();
	const RenderManager::Caps & GetCaps();
    
    const RenderManager::Stats & GetStats();
    void EnableOutputDebugStatsEveryNFrame(int32 frameToShowDebugStats);
    void ProcessStats();
    
	/** 
	 \brief Init FBO system function
	 \param[in] viewRenderbuffer
	 \param[in] viewFramebuffer
	 */
	/*
     GLuint should be equal size as uint32
     */
	void InitFBO(uint32 viewRenderbuffer, uint32 viewFramebuffer);
#if defined (__DAVAENGINE_OPENGL__)
    void InitGL20();
    void ReleaseGL20();
    Shader * colorOnly;
    Shader * colorWithTexture;
#endif 
    
protected:
	RenderManager::Caps caps;
    RenderManager::Stats stats;
    int32 statsFrameCountToShowDebug;
    int32 frameToShowDebugStats;
    Core::eRenderer renderer;
    
public:
    
    /**
        === Render Manager threading functions 
     */
	/** 
	 \brief 
	 */
	void Lock();
	/** 
	 \brief 
	 */
	void Unlock();
	/** 
	 \brief 
	 */
	void LockNonMain();
	/** 
	 \brief 
	 */
	void UnlockNonMain();
	
	
	int32 GetNonMainLockCount();
	
    
    /**
     === Viewport and orientation 
     */

	/** 
	 \brief 
	 \param[in] orientation
	 */
	void SetRenderOrientation(int32 orientation);
	/** 
	 \brief 
	 \returns 
	 */
	int32 GetRenderOrientation();
    
    /**
        \brief 
        
     */
    void SetViewport(const Rect & rect); 
       
    
    
    /**
     === State Management
     */

	/** 
	 \brief 
	 */
	void Reset();
	
	/** 
	 \brief 
	 */
	void FlushState();

	/** 
	 \brief 
	 \param[in] sfactor
	 \param[in] dfactor
	 */
	void SetBlendMode(eBlendMode sfactor, eBlendMode dfactor);
	eBlendMode GetSrcBlend();
	eBlendMode GetDestBlend();
	
	/** 
	 \brief 
	 \param[in] r
	 \param[in] g
	 \param[in] b
	 \param[in] a
	 */
	void SetColor(float32 r, float32 g, float32 b, float32 a);
	void SetColor(const Color & color);
	float32 GetColorR();
	float32 GetColorG();
    float32 GetColorB();
    float32 GetColorA();
    const Color & GetColor() const;
	void ResetColor();

	// 
	void SetTexture(Texture *texture, uint32 textureLevel = 0);
	Texture * GetTexture(uint32 textureLevel = 0);
    void SetShader(Shader * shader);
    Shader * GetShader();
	
	void EnableBlending(bool isEnabled);
	void EnableTexturing(bool isEnabled);
	
	void EnableVertexArray(bool isEnabled);
	void EnableTextureCoordArray(bool isEnabled);
	void EnableColorArray(bool isEnabled);

    bool IsDepthTestEnabled();
    bool IsDepthWriteEnabled();

    void EnableDepthTest(bool isEnabled);
    void EnableDepthWrite(bool isEnabled);
    
    void EnableAlphaTest(bool isEnabled);
    void SetAlphaFunc(eAlphaFunc func, float32 cmpValue);
    void EnableCulling(bool isEnabled);
    void SetCullFace(eCull cullFace);
    
    
    void SetRenderData(RenderDataObject * object);
    virtual void AttachRenderData(Shader * shader);
	
	/** 
	 \brief 
	 */
	void SetTexCoordPointer(int32 size, eVertexDataType type, int stride, const void *pointer);
    void SetVertexPointer(int32 size, eVertexDataType type, int stride, const void *pointer);
	void SetColorPointer(int32 size, eVertexDataType type, int stride, const void *pointer);
	void SetNormalPointer(eVertexDataType type, int stride, const void *pointer);

    
	/** 
        \brief 
	 */
	void DrawArrays(ePrimitiveType type, int32 first, int32 count);
	void HWDrawArrays(ePrimitiveType type, int32 first, int32 count);
	
	void DrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices); 
	void HWDrawElements(ePrimitiveType type, int32 count, eIndexFormat indexFormat, void * indices); 
		
	/** 
	 \brief Sets the clip rect
	 \param[in] rect
	 */
	virtual void SetClip(const Rect &rect);

	/** 
	 \brief Sets the clip rect as an intersection of the current rect and rect sent to method
	 \param[in] rect
	 */
	virtual void ClipRect(const Rect &rect);
	
	/** 
	 \brief Sets clip yo the full screen
	 */
	virtual void RemoveClip();

	/** 
	 \brief Store current clip
	 */
	virtual void ClipPush();

	/** 
	 \brief Restore current screen
	 */
	virtual void ClipPop();
	
	/** 
        \brief Clear rendering surface with required color 
        \param[in] r,g,b,a Clear color components
	 */
	virtual void ClearWithColor(float32 r, float32 g, float32 b, float32 a);\
    
    /** 
        \brief Clear attached depth buffer with requested depth
        \param[in] depth by default 1.0f, means clear the depth
     */
    virtual void ClearDepthBuffer(float32 depth = 1.0f);
	
	/** 
	 \brief Sets the sprite to use as a render target. Sprite should be created with CreateAsRenderTarget method.
			Call RestoreRenderTarget when you finish drawing to your sprite 
	 \param[in] renderTarget - Render target sprite. If NULL 0 render manager will draw to the screen.
	 */
	virtual void SetRenderTarget(Sprite *renderTarget);

	/** 
        \brief Restores the previous render target
	 */
	virtual void RestoreRenderTarget();

	/** 
	 \brief Checks is render target using for drawing now
	 \param[out] true if render manager sets to a render targe. false if render manager draws to the screen now
	 */
	virtual bool IsRenderTarget();
	
	/** 
        \brief Sets the effect for the rendering. 
        \param[in] renderEffect - if 0, sets the effect to none
	 */
	virtual void SetRenderEffect(RenderEffect *renderEffect);

	/** 
	 \brief Sets the requested framerate. For iPhone can be set to 60, 30, 20, 15
	 \param[in] newFps requested frames per second
	 */
	virtual void SetFPS(int32 newFps);

	/** 
	 \brief Returns current requested framerate
	 \returns frames per second
	 */
    virtual int32 GetFPS();

	virtual void SetDebug(bool isDebugEnabled);


	/** 
	 \brief 
	 \param[in] offset
	 */
	virtual void SetDrawTranslate(const Vector2 &offset);

	/** 
	 \brief 
	 \param[in] offset
	 */
	virtual void SetDrawScale(const Vector2 &scale);

	virtual void IdentityDrawMatrix();
	virtual void IdentityTotalMatrix();
	
	/*
		TODO:	Hottych - напиши пожалуйста что делают эти функции детально, 
				чтобы было понятно как и в каких случаях их надо использовать
				Думаю что пока воспоминания свежи, напиши документацию по системе виртуальных преобразований
				Можешь писать на русском - я переведу потом.
	 */
	virtual void SetPhysicalViewScale();
	virtual void SetPhysicalViewOffset();
	virtual void SetVirtualViewScale();
	virtual void SetVirtualViewOffset();

	virtual void PushDrawMatrix();
    virtual void PopDrawMatrix();

    virtual void PushMappingMatrix();
	virtual void PopMappingMatrix();
    
    
    
    /*  
        Matrix support
     */
    enum eMatrixType
    {
        MATRIX_MODELVIEW = 0,
        MATRIX_PROJECTION,
        MATRIX_COUNT,
    };
    
    enum eUniformMatrixType
    {
        UNIFORM_MATRIX_MODELVIEWPROJECTION = 0,
        UNIFORM_MATRIX_COUNT,
    };
    
    virtual void SetMatrix(eMatrixType type, const Matrix4 & matrix);
    virtual const Matrix4 & GetMatrix(eMatrixType type);
    virtual const Matrix4 & GetUniformMatrix(eUniformMatrixType type);
    virtual void  ClearUniformMatrices();


	/**
		\brief This function sets hardware cursor to render manager
		It acts differently in different operation systems but idea is common. 
		When you call this function on next refresh cursor will be changed to the new one.
	*/
	virtual void SetCursor(Cursor * cursor);

	/**
		\brief This function get hardware cursor that actively set
		By default at application launch function returns zero. If this function returns zero 
		we use default cursor that is provided by operational system. 
		\returns pointer to custom cursor or null if there is no cursor set by default.
	 */
	virtual Cursor * GetCursor();
	
protected:
    //
    // general matrices for rendermanager 
    // 
    
    Matrix4 matrices[MATRIX_COUNT];
    int32   uniformMatrixFlags[UNIFORM_MATRIX_COUNT];
    Matrix4 uniformMatrices[UNIFORM_MATRIX_COUNT];
    

    void RectFromRenderOrientationToViewport(Rect & rect);
    
	// SHOULD BE REPLACED WITH MATRICES IN FUTURE
	Vector2 userDrawOffset;
	Vector2 userDrawScale;

	//need to think about optimization two matrices (userDraw matrix and mapping matrix) to the one matrix
	Vector2 viewMappingDrawOffset;
	Vector2 viewMappingDrawScale;

	Vector2 realDrawOffset;
	Vector2 realDrawScale;
	
	Vector2 currentDrawOffset;
	Vector2 currentDrawScale;
	
	void PrepareRealMatrix();

	
	
	/** 
	 \brief 
	 \returns 
	 */
	int32 GetScreenWidth();
	
	/** 
	 \brief 
	 \returns 
	 */
	int32 GetScreenHeight();
	
	
	typedef struct RenderTarget_t 
		{
			Sprite *spr;
			int orientation;
		} RenderTarget;

	typedef struct DrawMatrix_t 
	{
		Vector2 userDrawOffset;
		Vector2 userDrawScale;
	} DrawMatrix;
	

	// fbo data
	uint32 fboViewRenderbuffer;
	uint32 fboViewFramebuffer;

	// state information
	Color oldColor;                 // UNIFORM - can be used or not used by RenderEffect
	Color newColor;                 // UNIFORM - can be used or not used by RenderEffect
    
	eBlendMode oldSFactor, oldDFactor;  // STATE
	eBlendMode newSFactor, newDFactor;  // STATE
    
    static const uint32 MAX_TEXTURE_LEVELS = 4;
	Texture *currentTexture[MAX_TEXTURE_LEVELS];                        // Texture that was set
    Shader * shader;
	
    int newTextureEnabled, oldTextureEnabled;       // Enable or disable texturing
	int oldVertexArrayEnabled;                      // state
	int oldTextureCoordArrayEnabled;                // state
	int oldColorArrayEnabled;                       // state
	int oldBlendingEnabled;                         // state
    int depthWriteEnabled;                          // state
    int depthTestEnabled;                           // state
    
    bool oldAlphaTestEnabled;                       // default value: false
    bool alphaTestEnabled;                          // default value: false
    eAlphaFunc alphaFunc;                           // 
    eAlphaFunc oldAlphaFunc;
    float32 oldAlphaTestCmpValue;                   // old alpha test cmp value
    float32 alphaTestCmpValue;                      // default value: 0.0f
    bool cullingEnabled, oldCullingEnabled;
    eCull cullFace, oldCullFace;
    
    
    uint32 pointerArraysCurrentState;
    uint32 pointerArraysRendererState;
    uint32 enabledAttribCount;

    
    
    int32 renderOrientation;
	Sprite *currentRenderTarget;
	std::stack<Rect> clipStack;
	std::stack<RenderTarget> renderTargetStack;
	std::stack<DrawMatrix> matrixStack;
	std::stack<DrawMatrix> mappingMatrixStack;

	std::stack<RenderEffect*> renderEffectStack;
	RenderEffect *currentRenderEffect;
	
    RenderDataObject * currentRenderData;

    /* 
        Size of the original renderTarget created for 3D rendering.
    */
    
	int32 frameBufferWidth;
	int32 frameBufferHeight;
	int32 retScreenWidth;
	int32 retScreenHeight;
	
	int32 fps;

	int32 lockCount;
	bool isInsideDraw;

	Mutex glMutex;
	
	Rect currentClip;
	
	void SetHWRenderTarget(Sprite *renderTarget);
	void SetHWClip(const Rect &rect);

	void SetNewRenderEffect(RenderEffect *renderEffect);
	
	bool debugEnabled;
	
#if defined(__DAVAENGINE_DIRECTX9__)
	// 
	bool scissorTestEnabled;
	struct BufferData
	{	
		BufferData()
		{	
			isEnabled = false;
			size = 0;
			type = TYPE_FLOAT;
			stride = 0;
			pointer = 0;
		}


		bool isEnabled;
		int32 size;
		eVertexDataType	type;
		int32 stride;
		const void * pointer;
	};
	enum
	{
		BUFFER_VERTEX = 0,
		BUFFER_COLOR,
		BUFFER_TEXCOORD0,
		BUFFER_TEXCOORD1,
		BUFFER_TEXCOORD2,
		BUFFER_TEXCOORD3,
		BUFFER_NORMAL,

		BUFFER_COUNT
	};

	BufferData buffers[BUFFER_COUNT];
	D3DCAPS9 deviceCaps;
#endif // #if defined(__DAVAENGINE_DIRECTX9__)
	
	Cursor * cursor;
};


};
#endif
