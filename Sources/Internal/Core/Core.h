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
#ifndef __DAVAENGINE_CORE_H__
#define __DAVAENGINE_CORE_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/Singleton.h"
#include "Core/ApplicationCore.h"
#include "Core/DisplayMode.h"
#include "FileSystem/KeyedArchive.h"

/**
	\defgroup core SDK Application Basics
	This is group that describe SDK application core architecture.
*/
namespace DAVA 
{
	
	
#if defined(__DAVAENGINE_WIN32__)
	typedef HINSTANCE AppHandle;
#else
	typedef uint32 AppHandle;
#endif 
	
/**
	\ingroup core
	\brief	Core SDK class that initialize everything under all of platforms. It's common place where you can get some specific information about your application on every supported platform. 
			To read about the process of application initialization check documentation for \ref ApplicationCore class. 
			
 
			Supported engine configuration options: 
				
			\section w32_macos Win32 / MacOS X 
			width: 1024<br/>
			height: 768<br/>
			fullscreen: 1<br/>
			bitsperpixel: 32<br/>
			 
			\section ios iOS
			orientation:	SCREEN_ORIENTATION_LANDSCAPE_RIGHT,
							SCREEN_ORIENTATION_LANDSCAPE_LEFT,
							SCREEN_ORIENTATION_PORTRAIT,
							SCREEN_ORIENTATION_PORTRAIT_UPSIDE_DOWN<br/>
 
            renderer:         
                RENDERER_OPENGL_ES_1_0, 
                RENDERER_OPENGL_ES_2_0, 
                RENDERER_OPENGL,        
                RENDERER_DIRECTX9       <br/>
           
			
			\section all All platforms
			zbuffer: 1	<br/>

			Specific implementation notes (for people who involved to development of platform dependant templates)
			Core::CreateSingletons must be always called from main thread of application or from main rendering thread.
			It's required to perform thread system initialization correctly. 
 */
class Core : public Singleton<Core>
{
public:
	
	struct AvailableSize
	{
		AvailableSize()
			:	width(0)
			,	height(0)
			,	toVirtual(0)
			,	toPhysical(0)
		{

		}
		int32 width;
		int32 height;
		String folderName;
		float32 toVirtual;
		float32 toPhysical;
	};
	
	enum eScreenOrientation
	{
			SCREEN_ORIENTATION_TEXTURE = -1// uses only for the draw to texture purposes
		,	SCREEN_ORIENTATION_LANDSCAPE_RIGHT = 0
		,	SCREEN_ORIENTATION_LANDSCAPE_LEFT
		,	SCREEN_ORIENTATION_PORTRAIT
		,	SCREEN_ORIENTATION_PORTRAIT_UPSIDE_DOWN
	};
    
    enum eRenderer
    {
        RENDERER_OPENGL_ES_1_0, // 1.0 compatible OpenGL ES. Old generation iOS / Android devices. 
        RENDERER_OPENGL_ES_2_0, // 2.0 compatible OpenGL ES. New generation iOS / Android devices. 
        RENDERER_OPENGL,        // here we assuming that it's 2.0 compatible. Renderer for MacOS X. 
        RENDERER_DIRECTX9,      // only renderer that works on win platforms right now. 
//        RENDERER_DIRECTX10,   // written for self-motivation
//        RENDERER_DIRECTX11,   // written for self-motivation
    };
    
	
	Core();
	virtual ~Core();
	
	enum eScreenMode
	{
		MODE_UNSUPPORTED = 0,	// for all devices that do not support 
		MODE_FULLSCREEN, 
		MODE_WINDOWED,
	};
	
	static int Run(int argc, char *argv[], AppHandle handle = 0);
	static int RunCmdTool(int argc, char *argv[], AppHandle handle = 0);

	// Should be called in platform initialization before FrameworkDidLaunched
	void CreateSingletons();
    // Should be called after framework did launched to initialize proper render manager
    void CreateRenderManager();
    // Should be called after full release
	void ReleaseSingletons();

	Vector<String> & GetCommandLine(); 
	bool IsConsoleMode();
	
public:
	static void SetOptions(KeyedArchive * archiveOfOptions);
	static KeyedArchive * GetOptions();

	
	static void SetApplicationCore(ApplicationCore * core);
	static ApplicationCore * GetApplicationCore();

	
	// platform dependant functions that should be implemented
	virtual eScreenMode GetScreenMode();	// 
	
	/**
		\brief This function should perform switching from one mode to another (fullscreen => windowed and back)
		\param[in] screenMode mode of the screen we want to switch to
	*/
	virtual void SwitchScreenToMode(eScreenMode screenMode); 
	
	/**
		\brief Get list of available display modes supported by hardware
		\param[out] availableModes list of available modes that is supported by hw
	*/
	virtual void GetAvailableDisplayModes(List<DisplayMode> & availableModes);
	
	/**
		
	*/
	virtual void ToggleFullscreen();

	/**
		\brief Find mode that matches best to the mode you've requested
		\param[in] requestedMode mode you want to get
		\returns best mode found in current HW
	*/
	virtual DisplayMode FindBestMode(const DisplayMode & requestedMode);

	/**
		\brief Get current display mode. This function return resolution of the current display mode enabled on the first (main) monitor
	*/
	virtual DisplayMode GetCurrentDisplayMode();

	/**
		\brief Quit from application & release all subsystems
	*/
	virtual void Quit();
	
#if defined(__DAVAENGINE_IPHONE__)
	static bool IsAutodetectContentScaleFactor();
#endif 
//	static void SetContentScaleFactor(float scaleFactor);//sets content scale factor
//	static float GetContentScaleFactor();//returns content scale factor
//	static float GetInverseContentScaleFactor();//returns one divided by content scale factor(0.5 for scale factor 2.0)
	
	static float32 GetVirtualToPhysicalFactor();
	static float32 GetPhysicalToVirtualFactor();
	
	
	virtual Core::eScreenOrientation GetScreenOrientation();
	virtual void CalculateScaleMultipliers();
	
	virtual void SetPhysicalScreenSize(int32 width, int32 height);//!< May be used only by the system
	virtual void SetVirtualScreenSize(int32 width, int32 height);// Sets virtual screen size. You need to set size what takes into account screen orientation modifer
	virtual void RegisterAvailableResourceSize(int32 width, int32 height, const String &resourcesFolderName);// Registers available sizes of resources. Can be called many times.
	

	virtual float32 GetPhysicalScreenWidth();//returns physical size what don't take intpo account screen orientation
	virtual float32 GetPhysicalScreenHeight();//returns physical size what don't take intpo account screen orientation
	virtual const Vector2 &GetPhysicalDrawOffset();

	virtual float32 GetVirtualScreenWidth();
	virtual float32 GetVirtualScreenHeight();
	virtual float32 GetVirtualScreenXMin();
	virtual float32 GetVirtualScreenXMax();
	virtual float32 GetVirtualScreenYMin();
	virtual float32 GetVirtualScreenYMax();
	
	virtual float32 GetResourceToPhysicalFactor(int32 resourceIndex);
	virtual float32 GetResourceToVirtualFactor(int32 resourceIndex);
	virtual const String& GetResourceFolder(int32 resourceIndex);
	virtual int32 GetDesirableResourceIndex();
	virtual int32 GetBaseResourceIndex();
	
	
	/*
		\brief Mouse cursor for the platforms where it make sense (Win32, MacOS X) 
	 */

	
	/* This code disabled for now and left for the future
	MacOS X Version: it works right (commented in MainWindowController.mm) but it require convertaton to virtual coordinates
	For Win32 function not implemented yet, and I do not have time to implement it right now, so left that for the future.

#if defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_WIN32__)
	const Vector2 & GetMouseLocation();
#endif 
	*/

	/*
		\brief Function that return number of frame from the launch of the application
		
		This function supposed for such situations when you do not want to recompute something during one frame more than 
		once. So you can store frameIndex in your object and check have you updated it already or not. 
		By default this counter starts from frame with index 1, so you can initialize your variables by 0 if you want to 
		use this index. 
		
		Usage example: 
		\code
		uint32 updateFrameIndex = 0;
	 
		void UpdateFunction()
		{
			if (updateFrameIndex == )return; // no update
			updateCounter = Core::Instance()->GetGlobalFrameIndex();
	 
		}
	 
		\endcode
		
		\returns global frame index from the launch of your application
	 */
	uint32 GetGlobalFrameIndex();
	
	/*
		This function performs message on main thread 
		\param[in] message message to be performed
	 */
	//void PerformMessageOnMainThread(const Message & message, bool waitUntilDone = true);
	
	/*
		* FOR INTERNAL FRAMEWORK USAGE ONLY * 
		MUST BE CALLED FROM templates on different OS
	 */
	
	void SystemAppStarted();
	void SystemProcessFrame();
	void SystemAppFinished();
	
	virtual void Suspend();
	virtual void Resume();
	virtual void GoBackground();	
	
	
	/**
		\brief Checks if framework needs to recalculate scale multipliers.
	*/
	bool NeedToRecalculateMultipliers();

private:
	int32 screenOrientation;
	float32 screenWidth;
	float32 screenHeight;
	
	float32 rotatedScreenWidth;
	float32 rotatedScreenHeight;
	
	int desirableIndex;
	
	float32 virtualScreenWidth;
	float32 virtualScreenHeight;
	
	Vector<AvailableSize> allowedSizes;
	bool needTorecalculateMultipliers;
	
	static float32 virtualToPhysical;
	static float32 physicalToVirtual;
	static Vector2 drawOffset;
	
	bool isActive;
	
	uint32 globalFrameIndex;

	bool firstRun;//call begin frame 1st time
	
	void EnableConsoleMode();
	void SetCommandLine(int argc, char *argv[]);
	Vector<String> commandLine;
	bool isConsoleMode;
    
    void CheckDataTypeSizes();
    template <class T> void CheckType(T t, int32 expectedSize, const char * typeString);
};

float32 GetScreenWidth();
float32 GetScreenHeight();

};



#endif // __DAVAENGINE_CORE_H__
